#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <json-c/json.h>
#include "settings.h"
#include "JSONParser.h"
#include "mongoose.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "JSONParser.h"


#define FILE_NAME_INFOMATION "/home/root/www/information.json"
std::shared_ptr<spdlog::logger> file_logger = spdlog::stdout_color_mt("nm_logger");

// Static variables
static int s_debug_level = MG_LL_INFO;
static const char *s_root_dir = "/home/root/www";
static const char *s_listening_address = "http://0.0.0.0:9000";
static const char *s_enable_hexdump = "no";
static const char *s_ssi_pattern = ".html";
static const char *s_upload_dir = "/home/root/www";
static int s_signo;



// Signal handler
static void signal_handler(int signo) {
    s_signo = signo;
}

// Event handler for HTTP connection
static void cb(struct mg_connection *c, int ev, void *ev_data,  void *fn_data) {
//	Settings settings(file_logger);
//	Settings *server_ =static_cast<Settings*>(fn_data); // Cast void* back to class instance
	Settings settings_(file_logger);
//    if (!server_) {
//    	spdlog::error("Invalid server instance in handleEvent");
//        return;
//    }

	JSONParser parser = settings_.parser;
	Information info = parser.parseInformation(FILE_NAME_INFOMATION);
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        spdlog::info("Received request for: {}", std::string(hm->uri.ptr, hm->uri.len));

        if (mg_http_match_uri(hm, "/")) {
            struct mg_http_serve_opts opts = {0};
            mg_http_serve_file(c, hm, info.getLoginPath().c_str(), &opts);
        } else if (mg_http_match_uri(hm, "/login")) {
            char username[100], password[100];
            mg_http_get_var(&hm->body, "username", username, sizeof(username));
            mg_http_get_var(&hm->body, "password", password, sizeof(password));

            if (settings_.compareUsernamePassword(username,password) == 0) {
                struct mg_http_serve_opts opts = {0};
                opts.root_dir = s_root_dir;
//                mg_http_serve_file(c, hm, info.getHomePath().c_str(), &opts);
                mg_http_reply(c, 200, "", "<html><head><script>alert('Login successfully!'); window.location.href = 'home.html';</script></head><body></body></html>");
            } else {
                mg_http_reply(c, 401, "", "Unauthorized");
            }
        } else if (mg_http_match_uri(hm, "/update")) {
            // Handle /update
        	char ip_address[100], logging_level[100], wireless_mode[100], wireless_SSID[100], wireless_passphrase[100];
        	char ip_select[100], gateway[100], dns[100];
        	// Parse form data
			mg_http_get_var(&hm->body, "ip-address-manual", ip_address, sizeof(ip_address));
			mg_http_get_var(&hm->body, "logging-level", logging_level, sizeof(logging_level));
			mg_http_get_var(&hm->body, "wireless-mode", wireless_mode, sizeof(wireless_mode));
			mg_http_get_var(&hm->body, "wireless-SSID", wireless_SSID, sizeof(wireless_SSID));
			mg_http_get_var(&hm->body, "wireless-Pass-Phrase", wireless_passphrase, sizeof(wireless_passphrase));
			mg_http_get_var(&hm->body, "gateway-manual", gateway, sizeof(gateway));
			mg_http_get_var(&hm->body, "dns-manual", dns, sizeof(dns));
			mg_http_get_var(&hm->body, "ip-select", ip_select, sizeof(ip_select));
			// update JSON data
			spdlog::info("Wireless mode: {}", wireless_mode);
			spdlog::info("ip-select: {}",ip_select);

			if(strcmp(wireless_mode,"station") == 0) {
				settings_.updateDataJsonSTA(ip_address,logging_level,wireless_mode,wireless_SSID,wireless_passphrase,ip_select);
				int ret = settings_.switchToSTAMode(wireless_SSID,wireless_passphrase,ip_address,gateway,dns,ip_select);
				if(ret != 0) {
					int swicth = settings_.switchToAPMode();
					if(swicth != 0) {
						settings_.switchToAPMode();
					}
				}
			}
			else {
				settings_.updateDataJsonAp();
				settings_.switchToAPMode();
			}
			struct mg_http_serve_opts opts = {0};
			opts.root_dir = s_root_dir;
			mg_http_serve_file(c, hm, info.getSettingsPath().c_str(), &opts);
        } else if (mg_http_match_uri(hm, "/change_password")) {

        	char password[100];
			mg_http_get_var(&hm->body, "new_password", password, sizeof(password));
			settings_.updatePasswordJson(password);
			mg_http_reply(c, 200, "", "<html><head><script>alert('Password changed successfully!'); window.location.href = 'change_password.html';</script></head><body></body></html>");
        } else if (mg_http_match_uri(hm, "/upload")) {
            // Handle /upload
        } else if (mg_http_match_uri(hm, "/download")) {
        	FILE *fp = fopen(info.getLogfile1Path().c_str(), "rb");
			if (fp != NULL) {
				fseek(fp, 0, SEEK_END);
				long file_size = ftell(fp);
				fseek(fp, 0, SEEK_SET);

				char *file_content = (char *)malloc(file_size);
				if (file_content != NULL) {
					fread(file_content, 1, file_size, fp);
					fclose(fp);

					mg_http_reply(c, 200, "Content-Type: application/octet-stream\r\nContent-Disposition: attachment; filename=\"logs.txt\"\r\n", "%.*s", (int)file_size, file_content);

					free(file_content);
				} else {
					fclose(fp);
					mg_http_reply(c, 500, "", "Error reading file.");
				}
			} else {
				mg_http_reply(c, 500, "", "Error opening file.");
			}

        } else {
            struct mg_http_serve_opts opts = {0};
            opts.root_dir = s_root_dir;
            opts.ssi_pattern = s_ssi_pattern;
            mg_http_serve_dir(c, hm, &opts);
        }
 //       spdlog::info("{} {} {} -> {} {}", std::string(hm->method.ptr, hm->method.len),
 //                    std::string(hm->uri.ptr, hm->uri.len), hm->body.len, 3, c->send.buf + 9,
 //                    c->send.len);
    }
}

// Usage function
static void usage(const char *prog) {
    fprintf(stderr,
            "Mongoose v.%s\n"
            "Usage: %s OPTIONS\n"
            "  -H yes|no - enable traffic hexdump, default: '%s'\n"
            "  -S PAT    - SSI filename pattern, default: '%s'\n"
            "  -d DIR    - directory to serve, default: '%s'\n"
            "  -l ADDR   - listening address, default: '%s'\n"
            "  -u DIR    - file upload directory, default: unset\n"
            "  -v LEVEL  - debug level, from 0 to 4, default: %d\n",
            MG_VERSION, prog, s_enable_hexdump, s_ssi_pattern, s_root_dir,
            s_listening_address, s_debug_level);
    exit(EXIT_FAILURE);
}



int main(int argc, char *argv[]) {

	Settings settings(file_logger);
	int n_retry = 3;
	int ret = 0;
	// settings.Loading_Logger(file_logger);
	for(int i =0 ; i < n_retry; i ++){
		ret = settings.initializeJson();
		if(ret==0)
			break;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	if(ret != 0) {
		  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		settings.switchToAPMode();
	}
    char path[MG_PATH_MAX] = ".";
    struct mg_mgr mgr;
    struct mg_connection *c;
    int i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            s_root_dir = argv[++i];
        } else if (strcmp(argv[i], "-H") == 0) {
            s_enable_hexdump = argv[++i];
        } else if (strcmp(argv[i], "-S") == 0) {
            s_ssi_pattern = argv[++i];
        } else if (strcmp(argv[i], "-l") == 0) {
            s_listening_address = argv[++i];
        } else if (strcmp(argv[i], "-u") == 0) {
            s_upload_dir = argv[++i];
        } else if (strcmp(argv[i], "-v") == 0) {
            s_debug_level = atoi(argv[++i]);
        } else {
            usage(argv[0]);
        }
    }
    if (strchr(s_root_dir, ',') == NULL) {
        realpath(s_root_dir, path);
        s_root_dir = path;
    }
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    mg_log_set(s_debug_level);
    mg_mgr_init(&mgr);
    if ((c = mg_http_listen(&mgr, s_listening_address, cb, (void*)&settings)) == NULL) {
        spdlog::error("Cannot listen on {}. Use http://ADDR:PORT or :PORT", s_listening_address);
        exit(EXIT_FAILURE);
    }
    if (mg_casecmp(s_enable_hexdump, "yes") == 0) c->is_hexdumping = 1;

    spdlog::info("Mongoose version : v{}", MG_VERSION);
    spdlog::info("Listening on     : {}", s_listening_address);
    spdlog::info("Web root         : [{}]", s_root_dir);
    spdlog::info("Upload dir       : [{}]", s_upload_dir ? s_upload_dir : "unset");

    while (s_signo == 0) mg_mgr_poll(&mgr, 1000);

    mg_mgr_free(&mgr);
    spdlog::info("Exiting on signal {}", s_signo);
    spdlog::drop_all();
    return 0;
}
