/*
 * settings.cpp
 *
 *  Created on: Jul 1, 2024
 *      Author: haonqptit
 */
#include "settings.h"
#include "NetworkManagerWrapper.h"
#include <iostream>
#include <exception>
#define INFORMATION_FILE_NAME "/home/root/www/information.json"



Settings::Settings(std::shared_ptr<spdlog::logger> logger): logger_(logger){
	//std::cout << "Settings created " << std::endl;
 //   if (logger_) {
   //     logger_->info("Settings class created successfully.");
  //  }


}


void Settings::Loading_Logger(std::shared_ptr<spdlog::logger> logger){
//	if(!logger_)
		logger_ =logger ;
    if (logger_) {
        logger_->info("Loading_Logger created successfully.");
    }
}



Settings::~Settings(){
	//std::cout << "Settings deconstructed " << std::endl;
  //  if (logger_) {
  //      logger_->info("Settings class destructed successfully.");
   // }

}


int Settings::switchToAPMode() {
	// 1. Modify the file content
	//std::string filename = "/etc/NetworkManager/conf.d/99-unmanaged-devices.conf";

	//std::ofstream file(filename, std::ios::trunc); // Open file for writing and truncate existing content

	//if (!file.is_open()) {
	//	std::cerr << "Unable to open file!" << std::endl;
	//	return 1;
	//}

	// Write new content to the file
	//file << "[keyfile]\n"
	//	 << "unmanaged-devices=interface-name:wlan0\n";

	//file.close(); // Close the file

	// 2. Reload NetworkManager
	std::system("systemctl stop NetworkManager");

	std::this_thread::sleep_for(std::chrono::milliseconds(300));

//    std::system("reboot");
	// 3. Restart hostapd service
	int ret = std::system("ip addr add 192.168.10.1/24 dev wlan0");
	if (ret != 0) {
		std::cout << "Failed to ip addr add 192.168.10.1/24 dev wlan0 Error code: {} " << ret << std::endl;
		return ret;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(300));

	std::system("systemctl restart hostapd");
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	std::system("systemctl restart dnsmasq");
	std::cout << "Switch to AP mode sucssesful"  << std::endl;;
	return 0;
}

int Settings::switchToSTAMode(const std::string& ssid, const std::string& password, const std::string& static_ip, const std::string& gateway, const std::string& dns, const std::string& mode) {
	//logger_->info( "switchToSTAMode starting !");
	// 1. Stop hostapd service
	int ret = 0;
	ret = std::system("systemctl stop hostapd");
    if (ret != 0) {
    	std::cout <<  "Failed to stop hostapd service. Error code: "<< std::endl;
        return ret;
    }

	ret = std::system("systemctl stop dnsmasq");
    if (ret != 0) {
    	std::cout <<  "Failed to stop hostapd service. Error code: "<< std::endl;
        return ret;
    }

    // 2. Modify the file content
 //   std::string filename = "/etc/NetworkManager/conf.d/99-unmanaged-devices.conf";
  //  std::ofstream file(filename, std::ios::trunc); // Open file for writing and truncate existing content

 //   if (!file.is_open()) {
   // 	logger_->error( "Unable to open file: ");
  //      return 1;
 //   }

    // Write new content to the file
  //  file << "[keyfile]\n"
  //       << "#unmanaged-devices=interface-name:wlan0\n";

 //   file.close(); // Close the file

    // 3. Reload NetworkManager
    ret = std::system("systemctl restart NetworkManager");
    if (ret != 0) {
    	std::cout << "Failed to restart NetworkManager. Error code: {} " << ret << std::endl;
        return ret;
    }
    sleep(10);

    // 4. Connect to WiFi using nmcli

    if(mode == "auto") {
    	NetworkManagerWrapper nm_wrapper("wlan0", ssid, password, "", "", "", "MyConnection", logger_);
		if (nm_wrapper.connectToAccessPointWithAutoIP()) {
			std::cout << "Connected to access point with auto IP successfully." << std::endl;
		} else {
			std::cout << "Failed to connect to access point with auto IP." << std::endl;
			return 1;
		}
    }
    else {
        NetworkManagerWrapper nm_wrapper("wlan0", ssid, password, static_ip, gateway, dns, "Myconnection", logger_);
        // them truong gateway dns
        if (nm_wrapper.connectToAccessPointWithStaticIP()) {
        	std::cout << "Connected to access point with static IP successfully."<< std::endl;
        } else {
        	std::cout << "Failed to connect to access point with static IP."<< std::endl;
            return 1;
        }
    }
    std::cout << "Switch to STA mode sucssesful"<< std::endl;
    return 0;

}

int Settings::initializeJson() {

	int res = 0;
//	res = std::system("iw dev wlan0 interface add uap0 type __ap");
//    if (res != 0) {
//    	std::cout << "Failed to iw dev wlan0 interface add uap0 type __ap. Error code: {} " << res << std::endl;
//        return res;
//    }
//
//
//    res = std::system("ip addr add 192.168.10.1/24 dev uap0");
//    if (res != 0) {
//    	std::cout << "Failed to ip addr add 192.168.10.1/24 dev uap0 Error code: {} " << res << std::endl;
//        return res;
//    }
	//
	Information info = this->parser.parseInformation(INFORMATION_FILE_NAME);
	std::string configuration_file_name = info.getJsonPath();
	Configuration config = this->parser.parseConfiguration(configuration_file_name);
	// co thong tin cua congig roi, gio lay ra thoi
	std::string mode;
	if(config.getStaAuto() == "true") {
		mode = "auto";
	}
	else {
		mode = "manual";
	}
	std::string ap_status = config.getApStatus();
	std::string sta_status = config.getStaStatus();
	std::cout << "Status AP: " << ap_status << std::endl;
	std::cout << "Status STA: " << sta_status << std::endl;

	if(ap_status == "active") {
		 res = switchToAPMode();
	}
	if(sta_status == "active") {
		//res = switchToSTAMode(config.getStaWirelessSsid(),config.getApWirelessPassPhrase(), config.getApIpAddress(), config.getGateway(), config.getDns(), mode);
	    if(mode == "auto") {
	    	NetworkManagerWrapper nm_wrapper("wlan0", config.getStaWirelessSsid(), config.getApWirelessPassPhrase(), "", "", "", "MyConnection", logger_);
			if (nm_wrapper.connectToAccessPointWithAutoIP()) {
				std::cout << "Connected to access point with auto IP successfully." << std::endl;
			} else {
				std::cout << "Failed to connect to access point with auto IP." << std::endl;
				return 1;
			}
	    }
	    else {
	        NetworkManagerWrapper nm_wrapper("wlan0", config.getStaWirelessSsid(), config.getApWirelessPassPhrase(), config.getApIpAddress(), config.getGateway(),config.getDns(), "Myconnection", logger_);
	        // them truong gateway dns
	        if (nm_wrapper.connectToAccessPointWithStaticIP()) {
	        	std::cout << "Connected to access point with static IP successfully."<< std::endl;
	        } else {
	        	std::cout << "Failed to connect to access point with static IP."<< std::endl;
	            return 1;
	        }
	    }
	}

	return 0;
}

int Settings::updateDataJsonSTA(const std::string& ip_address, const std::string& logging_level,
		const std::string& wireless_mode, const std::string& wireless_SSID,
		const std::string& wireless_passphrase, const std::string& mode) {

	Information info = this->parser.parseInformation(INFORMATION_FILE_NAME);
	std::string configuration_file_name = info.getJsonPath();
	Configuration config = this->parser.parseConfiguration(configuration_file_name);
	config.setStaIpAddress(ip_address);
	config.setStaLoggingLevel(logging_level);
	config.setStaWirelessSsid(wireless_SSID);
	config.setStaWirelessMode(wireless_mode);
	config.setStaWirelessPassPhrase(wireless_passphrase);
	config.setStaStatus("active");
	config.setApStatus("deactive");
	if(mode == "auto") {
		config.setStaAuto("true");
	}
	else {
		config.setStaAuto("false");
	}
	this->parser.updateConfiguration(config, configuration_file_name);
//	int ret = switchToSTAMode(config.getStaWirelessSsid(), config.getStaWirelessPassPhrase());

	return 0;
}
int Settings::updateDataJsonAp() {
	Information info = this->parser.parseInformation(INFORMATION_FILE_NAME);
	std::string configuration_file_name = info.getJsonPath();
	Configuration config = this->parser.parseConfiguration(configuration_file_name);
	config.setApStatus("active");
	config.setStaStatus("deactive");
	this->parser.updateConfiguration(config, configuration_file_name);
	return 0;
}

int Settings::updatePasswordJson(const std::string& pass) {
	Information info = this->parser.parseInformation(INFORMATION_FILE_NAME);
	std::string configuration_file_name = info.getJsonPath();
	Configuration config = this->parser.parseConfiguration(configuration_file_name);
	config.setPassword(pass);
	this->parser.updateConfiguration(config, configuration_file_name);
	return 0;
}

int Settings::compareUsernamePassword(const std::string& userName, const std::string& passWord) {
	Information info = this->parser.parseInformation(INFORMATION_FILE_NAME);
	std::string configuration_file_name = info.getJsonPath();
	Configuration config = this->parser.parseConfiguration(configuration_file_name);

	std::string userNameFile = config.getUsername();
	std::string passWordFile = config.getPassword();

    if (userName == userNameFile && passWord == passWordFile) {
        return 0;
    } else {
        return 1;
    }
}


