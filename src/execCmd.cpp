// execCmd.cpp

#include "execCmd.hpp"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <curl/curl.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

ExecCmd::ExecCmd(const std::string& command) : command(command) {}

std::string ExecCmd::executeCommand() const {
    if (!command.empty()) {
        std::cout << "Executing command: " << command << std::endl;

        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        // popen exécute la commande dans un processus fils et crée un pipe entre le processus parent (le programme C++) et le processus fils (la commande shell)
        // la commande est exécutée dans le même environnement que le programme C++
        // D'autres stratégies sont possibles,  continuez d'experimenter, passer des pointeurs d'un processus à l'autre etc

        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }

        std::ostringstream resultStream;
        char buffer[128];
        // l'utilisateur peut dépasser du buffer
        // C++ met a disposition des classes et fonctions permettant
        // de minimiser le risque d'erreurs, utilisez les

        while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
            resultStream << buffer;
        }

        return resultStream.str();
    } else {
        std::cout << "No command to execute." << std::endl;
        return "";  
    }
}


void ExecCmd::sendToDiscordWebhook(const std::string& webhookURL, const std::string& message) const {
    CURL* curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Échapper le message pour les caractères spéciaux
        char* escapedMessage = curl_easy_escape(curl, message.c_str(), 0);
        // Le commentaire n'ajoute rien au code qui était déjà très clair.
        // Pire, il est menteur car les caractères tels que -, ., _, ou ~
        // sont considérés comme sûrs dans une URL et ne nécessitent donc
        // pas d’être échappés alors que ce sont bel et bien des caractères spéciaux.
        // Des noms comme slug ou urlEncoded à la place d'escapedMessage peuvent ajouter de la précision.

        std::string postData = "content=```" + std::string(escapedMessage) + "```";

        // Libérer la mémoire allouée par curl_easy_escape
        curl_free(escapedMessage);

        curl_easy_setopt(curl, CURLOPT_URL, webhookURL.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);  // Spécifier la méthode POST
        // Ce commentaire fait penser qu'il y a quelque chose de particulier à faire
        // attention avec la méthode POST. Pourquoi commenter ici et pas les autres?

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}