#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    vector<string> backpack; 
    int choice = 0;
    string nameItem;

    cout << "=== VOXEL SURVIVAL INVENTORY ===" << endl;

    // The loop now breaks when choice is 4
    while (choice != 4) {
        cout << "\nAction Menu:" << endl;
        cout << "1. Collect Material (Mining)" << endl;
        cout << "2. Open Inventory (Backpack)" << endl;
        cout << "3. Craft Pickaxe (Needs Wood and Stone)" << endl;
        cout << "4. Exit Game" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            cout << "Enter the name of your collected items (Without Space): ";
            cin >> nameItem;
            
            backpack.push_back(nameItem);
            cout << "[+] " << nameItem << " has been added to your backpack!" << endl;
        }
        else if (choice == 2) {
            cout << "\n=== YOUR BACKPACK (" << backpack.size() << " Slots Used) ===" << endl;
            if (backpack.empty()) {
                cout << "[ Empty... No item yet. ]" << endl;
            } else {
                for (int i = 0; i < backpack.size(); i++) {
                    cout << "- " << backpack[i] << endl;
                }
            }
            cout << "=======================================" << endl;
        }
        else if (choice == 3) {
            bool hasWood = false;
            bool hasStone = false;

            // 1. Scan the backpack using a 'for' loop
            for (int i = 0; i < backpack.size(); i++) {
                if (backpack[i] == "Wood") {
                    hasWood = true;
                }
                if (backpack[i] == "Stone") {
                    hasStone = true;
                }
            }

            // 2. Crafting logic using AND (&&)
            if (hasWood && hasStone) {
                cout << "\n[!] CRAFTING SUCCESS: You crafted a Pickaxe!" << endl;
                backpack.push_back("Pickaxe"); 
            } else {
                cout << "\n[x] Not enough materials! You need 'Wood' and 'Stone'." << endl;
            }
        }
    }

    cout << "Saving data... See you later!" << endl;
    return 0;

    else if (choice == 3) {
        int woodIndex
    }

}