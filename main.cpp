#include <iostream>
#include <string>

using namespace std;

int main() {
    string CharacterName;
    int level;
    
    int hpPlayer = 100;
    int armor = 2;

    cout << "=== SERVER LOGIN ===" << endl;
    cout << "Put Character Name (Without Space): ";
    cin >> CharacterName; 

    cout << "Put Initial Level: ";
    cin >> level;

    cout << "\nLoading Your Character..." << endl;
    cout << "Character " << CharacterName << " has been spawned at level " << level << "!" << endl;

    string MonsterName = "Manya";
    int hpMonster = 40;  // Added Monster HP
    int baseDamage = 15; // Increased Manya's damage 
    
    cout << "\n==== BATTLE START!! ====" << endl;

    // --- BATTLE LOOP ---
    while (hpPlayer > 0 && hpMonster > 0) {
        cout << "\n-------------------------------------" << endl;
        cout << "HP " << CharacterName << ": " << hpPlayer << "  |  HP " << MonsterName << ": " << hpMonster << endl;
        cout << "-------------------------------------" << endl;
        
        // Player Action Menu
        cout << "Choose your action:" << endl;
        cout << "1. Attack" << endl;
        cout << "2. Heal (Use Potion)" << endl;
        cout << "Your choice (Type 1 or 2): ";
        
        int choice;
        cin >> choice; // The program pauses here to wait for your input

        cout << "\n";

        // Player's Turn Logic
        if (choice == 1) {
            cout << "[!] " << CharacterName << " attacks " << MonsterName << "!" << endl;
            hpMonster -= 15; // Manya takes 15 damage
        } 
        else if (choice == 2) {
            cout << "[+] " << CharacterName << " drinks a Potion! Restored 20 HP." << endl;
            hpPlayer += 20;
        } 
        else {
            cout << "[?] Invalid choice! You hesitated and missed your turn..." << endl;
        }

        // Monster's Turn (Manya attacks ONLY IF she is still alive)
        if (hpMonster > 0) {
            int totalDamage = baseDamage - armor;
            hpPlayer -= totalDamage;
            cout << "[!] " << MonsterName << " attacks back! You took " << totalDamage << " damage." << endl;
        }
    }

    // --- BATTLE END LOGIC (Triggers when someone's HP hits 0) ---
    cout << "\n=== BATTLE END ===" << endl;
    if (hpPlayer <= 0) {
        cout << "YOU DIED. GAME OVER!" << endl;
    } else {
        cout << "VICTORY! " << MonsterName << " has been defeated!" << endl;
    }

    return 0;
}