#include "Common.h"
#include "IDABot.h"
#include "MyAgent.h"
#include "Util.h"

#include "sc2utils/sc2_manage_process.h"
#include "sc2api/sc2_api.h"
#include "TechTreeImproved.h"

int main(int argc, char* argv[]) 
{
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) 
    {
        std::cout << "Unable to find or parse settings." << std::endl;
        return 1;
    }
    
    std::string botRaceString = "Terran";
    std::string enemyRaceString = "Random";
    std::string mapString = "InterloperTest.SC2MAP";

    int stepSize = 1;
    sc2::Difficulty enemyDifficulty = sc2::Difficulty::Easy;

    // Add the custom bot, it will control the players.
    MyAgent bot1;
    //IDABot bot1;
    //IDABot bot2;
    
    // WARNING: Bot logic has not been thorougly tested on step sizes > 1
    //          Setting this = N means the bot's onFrame gets called once every N frames
    //          The bot may crash or do unexpected things if its logic is not called every frame
    coordinator.SetStepSize(stepSize);
    coordinator.SetRealtime(false);

    coordinator.SetParticipants({
        sc2::CreateParticipant(Util::GetRaceFromString(botRaceString), &bot1),
        //sc2::CreateParticipant(Util::GetRaceFromString(botRaceString), &bot2)
        sc2::CreateComputer(Util::GetRaceFromString(enemyRaceString), enemyDifficulty),
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(mapString);

    // Step forward the game simulation.
    while (true) 
    {
        coordinator.Update();
    }

    return 0;
}