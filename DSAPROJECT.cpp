#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <queue>
#include <stack>
#include <unistd.h>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

void typePrint(const string& text, int delay = 20) {
    for (char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delay));
    }
}

void printTitle(const string& title) {
    cout << CYAN << BOLD;
    cout << "\n══════════════════════════════════════════════════════════════════════════════\n";
    cout << "                         🎬 " << title << " 🎬\n";
    cout << "══════════════════════════════════════════════════════════════════════════════" << RESET << endl;
}

struct User {
    string username;
    int age;
    float moneyBalance;
};

struct Movie {
    string name;
    int duration;
    int seats;
    int seatsBooked;
    float rating;
    string genre;
    float price;
    vector<bool> seatMap;

    Movie(string n, int d, int s, int sb, float r, string g, float p)
        : name(n), duration(d), seats(s), seatsBooked(sb), rating(r), genre(g), price(p) {
        seatMap = vector<bool>(s, false);
    }

    Movie() : duration(0), seats(0), seatsBooked(0), rating(0.0), price(0.0) {}
};

struct Node_User {
    User user;
    Node_User* next;
};

class UserList {
private:
    Node_User* top;

public:
    UserList() : top(nullptr) {}

    void pushUser(User user) {
        Node_User* newNode = new Node_User{user, top};
        top = newNode;
    }

    User popUser() {
        if (!top) {
            cerr << RED << "⚠️ Stack is empty. Cannot pop.\n" << RESET;
            return {"", 0, 0.0};
        }
        User poppedUser = top->user;
        Node_User* temp = top;
        top = top->next;
        delete temp;
        return poppedUser;
    }

    void displayUserInfo() const {
        if (top) {
            const User& u = top->user;
            cout << MAGENTA << "👤 User Info: " << RESET 
                 << u.username << " | Age: " << u.age 
                 << " | Balance: $" << fixed << setprecision(2) << u.moneyBalance << "\n";
        } else {
            cout << YELLOW << "No user info available.\n" << RESET;
        }
    }
};

struct Node_Movie {
    Movie movie;
    Node_Movie* next;
};

class MovieList {
private:
    Node_Movie* head;

public:
    MovieList() : head(nullptr) {}

    void addMovie(Movie movie) {
        Node_Movie* newNode = new Node_Movie{movie, nullptr};
        if (!head) head = newNode;
        else {
            Node_Movie* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
    }

    Node_Movie* findMovie(const string& name) const {
        Node_Movie* temp = head;
        while (temp) {
            if (temp->movie.name == name) return temp;
            temp = temp->next;
        }
        return nullptr;
    }

    void displayMoviesAlphabetically() {
        mergeSort(&head);
        Node_Movie* temp = head;
        printTitle("Available Movies");
        while (temp) {
            cout << GREEN << "🎞️  " << RESET << temp->movie.name 
                 << " | " << temp->movie.genre 
                 << " | Rating: " << temp->movie.rating 
                 << " | 💲" << temp->movie.price << endl;
            temp = temp->next;
        }
    }

    void displaySeats(const Movie& movie) const {
        cout << BLUE << "\n🎬 Seats for: " << movie.name << RESET << "\n";
        for (int i = 0; i < movie.seats; ++i) {
            if (i % 10 == 0) cout << "\n";
            if (movie.seatMap[i])
                cout << RED << setw(3) << i + 1 << "🟥 " << RESET;
            else
                cout << GREEN << setw(3) << i + 1 << "🟩 " << RESET;
        }
        cout << "\n";
    }

    bool selectSeats(Movie& movie, int num) {
        vector<int> chosen;
        cout << YELLOW << "\nEnter seat numbers to book (space-separated): " << RESET;
        for (int i = 0; i < num; ++i) {
            int seatNo;
            cin >> seatNo;
            if (seatNo < 1 || seatNo > movie.seats) {
                cout << RED << "❌ Invalid seat number.\n" << RESET;
                return false;
            }
            if (movie.seatMap[seatNo - 1]) {
                cout << RED << "❌ Seat " << seatNo << " already booked.\n" << RESET;
                return false;
            }
            chosen.push_back(seatNo - 1);
        }

        for (int seat : chosen)
            movie.seatMap[seat] = true;

        movie.seatsBooked += num;
        return true;
    }

    void deleteMovie(const string& name) {
        Node_Movie *temp = head, *prev = nullptr;
        while (temp) {
            if (temp->movie.name == name) {
                if (prev) prev->next = temp->next;
                else head = temp->next;
                delete temp;
                cout << GREEN << "✅ Movie deleted successfully.\n" << RESET;
                return;
            }
            prev = temp;
            temp = temp->next;
        }
        cout << RED << "❌ Movie not found.\n" << RESET;
    }

    // Merge Sort (alphabetical)
    Node_Movie* merge(Node_Movie* a, Node_Movie* b) {
        if (!a) return b;
        if (!b) return a;
        Node_Movie* result = nullptr;
        if (a->movie.name <= b->movie.name) {
            result = a;
            result->next = merge(a->next, b);
        } else {
            result = b;
            result->next = merge(a, b->next);
        }
        return result;
    }

    void split(Node_Movie* src, Node_Movie** front, Node_Movie** back) {
        Node_Movie* fast = src->next;
        Node_Movie* slow = src;
        while (fast) {
            fast = fast->next;
            if (fast) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        *front = src;
        *back = slow->next;
        slow->next = nullptr;
    }

    void mergeSort(Node_Movie** headRef) {
        Node_Movie* headNode = *headRef;
        if (!headNode || !headNode->next) return;
        Node_Movie *a, *b;
        split(headNode, &a, &b);
        mergeSort(&a);
        mergeSort(&b);
        *headRef = merge(a, b);
    }
};

// ====== UTILITY FUNCTIONS ======
User createUser() {
    User u;
    printTitle("User Registration");
    cout << "Enter Username: ";
    cin.ignore();
    getline(cin, u.username);
    cout << "Enter Age: ";
    cin >> u.age;
    cout << "Enter Balance ($): ";
    cin >> u.moneyBalance;
    return u;
}

void writeUserToFile(const User& u) {
    ofstream f("userprofile.txt", ios::app);
    if (f.is_open()) {
        f << u.username << " | Age: " << u.age 
          << " | Balance: $" << fixed << setprecision(2) << u.moneyBalance << "\n";
    }
}

void displayUserProfileFromFile() {
    printTitle("User Profiles");
    ifstream f("userprofile.txt");
    string line;
    while (getline(f, line)) cout << line << "\n";
    f.close();
}

void bookTicket(UserList& users, MovieList& movies) {
    User user = users.popUser();
    if (user.username.empty()) {
        cout << RED << "⚠️ No user found.\n" << RESET;
        return;
    }

    string movieName;
    cout << "Enter Movie Name to Book: ";
    cin.ignore();
    getline(cin, movieName);
    Node_Movie* node = movies.findMovie(movieName);

    if (!node) {
        cout << RED << "❌ Movie not found.\n" << RESET;
        users.pushUser(user);
        return;
    }

    Movie& m = node->movie;
    movies.displaySeats(m);

    int num;
    cout << "\nEnter number of tickets: ";
    cin >> num;
    float cost = num * m.price;

    if (cost > user.moneyBalance) {
        cout << RED << "❌ Insufficient balance.\n" << RESET;
        users.pushUser(user);
        return;
    }

    if (movies.selectSeats(m, num)) {
        user.moneyBalance -= cost;
        users.pushUser(user);
        typePrint(GREEN + string("\n🎉 Booking Successful! Total: $") 
                  + to_string(cost) + "\n" + RESET, 15);
    } else {
        cout << RED << "❌ Booking failed.\n" << RESET;
        users.pushUser(user);
    }
}

void checkSeats(const MovieList& movies) {
    string name;
    cout << "Enter Movie Name: ";
    cin.ignore();
    getline(cin, name);
    Node_Movie* node = movies.findMovie(name);
    if (node) movies.displaySeats(node->movie);
    else cout << RED << "❌ Movie not found.\n" << RESET;
}

void displayMenu(UserList& users, MovieList& movies) {
    int choice;
    bool exitFlag = false;
    do {
        printTitle("MOVIE TICKET BOOKING SYSTEM");
        cout << YELLOW;
        cout << "[1] 🎟️  Book Ticket\n";
        cout << "[2] 🎞️  Show Movies List\n";
        cout << "[3] 💺  Check Seats\n";
        cout << "[4] 👤  User Profiles\n";
        cout << "[5] ❌ Exit\n";
        cout << RESET << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: bookTicket(users, movies); break;
            case 2: movies.displayMoviesAlphabetically(); break;
            case 3: checkSeats(movies); break;
            case 4: displayUserProfileFromFile(); break;
            case 5: exitFlag = true; break;
            default: cout << RED << "❌ Invalid choice.\n" << RESET;
        }
    } while (!exitFlag);
}

int main() {
    printTitle("WELCOME TO MOVIE TICKET BOOKING");
    usleep(300000);

    UserList users;
    MovieList movies;

    movies.addMovie({"Inception", 148, 50, 0, 8.8, "Sci-Fi", 12});
    movies.addMovie({"The Dark Knight", 152, 60, 0, 9.0, "Action", 10});
    movies.addMovie({"Interstellar", 169, 80, 0, 8.6, "Adventure", 15});
    movies.addMovie({"Avatar", 162, 70, 0, 7.8, "Fantasy", 20});
    movies.addMovie({"Titanic", 195, 55, 0, 7.9, "Romance", 18});
    movies.addMovie({"The Matrix", 136, 65, 0, 8.7, "Sci-Fi", 14});
    movies.addMovie({"Joker", 122, 75, 0, 8.5, "Drama", 16});
    movies.addMovie({"Avengers: Endgame", 181, 90, 0, 8.4, "Action", 25});
    movies.addMovie({"Spider-Man: No Way Home", 148, 85, 0, 8.3, "Action", 22});
    movies.addMovie({"Frozen II", 103, 45, 0, 6.9, "Animation", 12});
    movies.addMovie({"The Lion King", 118, 50, 0, 8.5, "Animation", 15});
    movies.addMovie({"Parasite", 132, 70, 0, 8.6, "Thriller", 14});
    movies.addMovie({"Black Panther", 134, 60, 0, 7.3, "Action", 20});
    movies.addMovie({"Doctor Strange", 115, 65, 0, 7.5, "Fantasy", 17});
    movies.addMovie({"Oppenheimer", 180, 95, 0, 8.9, "Biography", 15});
    movies.addMovie({"The Batman", 176, 85, 0, 8.1, "Action", 18});
    movies.addMovie({"Guardians of the Galaxy", 121, 70, 0, 8.0, "Sci-Fi", 16});
    movies.addMovie({"Dune", 155, 100, 0, 8.2, "Adventure", 10});
    movies.addMovie({"The Shawshank Redemption", 142, 50, 0, 9.3, "Drama", 12});
    movies.addMovie({"Fight Club", 139, 55, 0, 8.8, "Thriller", 15});

    User u = createUser();
    users.pushUser(u);
    writeUserToFile(u);

    displayMenu(users, movies);

    typePrint(GREEN + string("🎬 Thank you for using our system!\n") + RESET, 15);
    return 0;
}
