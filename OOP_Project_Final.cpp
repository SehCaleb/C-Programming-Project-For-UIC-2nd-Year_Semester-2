/*
 * Restaurant Management System (C++ / OOP)
 * Author  : Seh Favour Caleb Talle
 * UID     : 25BCD10313
 * Section : 25BCD-3B
 *
 * OOP pillars demonstrated:
 *   Encapsulation - private data + getters (MenuItem, Order, Restaurant)
 *   Abstraction   - abstract base class MenuItem (pure virtual functions)
 *   Inheritance   - Food, Beverage, Dessert derive from MenuItem
 *   Polymorphism  - virtual getPrice()/getCategory(), operator<< overloading
 *
 * Build: g++ -std=c++14 -Wall -o restaurant restaurant.cpp
 * Run  : ./restaurant   (Windows: restaurant.exe)
 */
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <limits>
using namespace std;

// ---------- ABSTRACTION: abstract base class ----------
class MenuItem {
private:                                   // ENCAPSULATION: data hidden
    int id;
    string name;
    double basePrice;
public:
    MenuItem(int i, string n, double p) : id(i), name(move(n)), basePrice(p) {}
    virtual ~MenuItem() = default;
    int getId() const { return id; }
    const string& getName() const { return name; }
    double getBasePrice() const { return basePrice; }
    virtual double getPrice() const = 0;       // pure virtual
    virtual string getCategory() const = 0;    // pure virtual
    void display() const {
        cout << left << setw(5) << id << setw(24) << name << setw(16) << getCategory()
             << "Rs. " << fixed << setprecision(2) << getPrice() << '\n';
    }
};

// ---------- INHERITANCE + POLYMORPHISM ----------
class Food : public MenuItem {
    bool vegetarian;
public:
    Food(int i, string n, double p, bool veg) : MenuItem(i, move(n), p), vegetarian(veg) {}
    double getPrice() const override { return getBasePrice() * 1.05; }   // 5% GST
    string getCategory() const override { return vegetarian ? "Food (Veg)" : "Food (Non-Veg)"; }
};
class Beverage : public MenuItem {
public:
    using MenuItem::MenuItem;
    double getPrice() const override { return getBasePrice() * 1.12; }   // 12% GST
    string getCategory() const override { return "Beverage"; }
};
class Dessert : public MenuItem {
public:
    using MenuItem::MenuItem;
    double getPrice() const override { return getBasePrice() * 1.08; }   // 8% GST
    string getCategory() const override { return "Dessert"; }
};

// ---------- ORDER ----------
class Order {
    static int counter;
    int orderId;
    string customer;
    int table;
    vector<pair<const MenuItem*, int>> lines;
public:
    Order(string c, int t) : orderId(++counter), customer(move(c)), table(t) {}
    void addItem(const MenuItem* m, int qty) {
        if (qty <= 0) throw invalid_argument("Quantity must be positive.");
        lines.emplace_back(m, qty);
    }
    bool empty() const { return lines.empty(); }
    int getId() const { return orderId; }
    double total() const {
        double t = 0;
        for (const auto& l : lines) t += l.first->getPrice() * l.second;  // polymorphic call
        return t;
    }
    friend ostream& operator<<(ostream& os, const Order& o) {           // operator overloading
        os << "\n========== BILL (Order #" << o.orderId << ") ==========\n"
           << "Customer: " << o.customer << "   Table: " << o.table << "\n"
           << string(40, '-') << '\n';
        for (const auto& l : o.lines)
            os << left << setw(22) << l.first->getName() << "x" << setw(3) << l.second
               << right << setw(10) << fixed << setprecision(2) << l.first->getPrice() * l.second << '\n';
        os << string(40, '-') << '\n' << left << setw(25) << "TOTAL (incl. GST)"
           << right << setw(10) << o.total() << "\n========================================\n";
        return os;
    }
};
int Order::counter = 0;

// ---------- RESTAURANT (manager class) ----------
class Restaurant {
    string name;
    vector<unique_ptr<MenuItem>> menu;
    vector<Order> orders;
public:
    explicit Restaurant(string n) : name(move(n)) {}
    void addMenuItem(unique_ptr<MenuItem> m) { menu.push_back(move(m)); }
    const MenuItem* findItem(int id) const {
        for (const auto& m : menu) if (m->getId() == id) return m.get();
        throw out_of_range("Item ID " + to_string(id) + " not found.");
    }
    void showMenu() const {
        cout << "\n--------- " << name << " MENU ---------\n"
             << left << setw(5) << "ID" << setw(24) << "Item" << setw(16) << "Category" << "Price\n";
        for (const auto& m : menu) m->display();
    }
    void takeOrder() {
        string cust; int table;
        cout << "Customer name: "; cin >> ws; getline(cin, cust);
        cout << "Table number: "; cin >> table;
        Order o(cust, table);
        while (true) {
            int id, qty;
            cout << "Item ID (0 to finish): "; cin >> id;
            if (id == 0) break;
            cout << "Quantity: "; cin >> qty;
            try { o.addItem(findItem(id), qty); cout << "  Added.\n"; }
            catch (const exception& e) { cout << "  Error: " << e.what() << '\n'; }
        }
        if (o.empty()) { cout << "Empty order discarded.\n"; return; }
        cout << o;
        orders.push_back(o);
    }
    void showSales() const {
        double sum = 0;
        cout << "\n--- Sales Summary ---\n";
        for (const auto& o : orders) {
            cout << "Order #" << o.getId() << "  Rs. " << fixed << setprecision(2) << o.total() << '\n';
            sum += o.total();
        }
        cout << "Orders: " << orders.size() << "   Revenue: Rs. " << sum << '\n';
    }
};

int main() {
    Restaurant r("Spice Garden");
    r.addMenuItem(make_unique<Food>(1, "Paneer Butter Masala", 220, true));
    r.addMenuItem(make_unique<Food>(2, "Chicken Biryani", 260, false));
    r.addMenuItem(make_unique<Food>(3, "Veg Fried Rice", 150, true));
    r.addMenuItem(make_unique<Beverage>(4, "Masala Chai", 40));
    r.addMenuItem(make_unique<Beverage>(5, "Cold Coffee", 90));
    r.addMenuItem(make_unique<Dessert>(6, "Gulab Jamun", 70));
    r.addMenuItem(make_unique<Dessert>(7, "Ice Cream", 80));

    int choice;
    do {
        cout << "\n1. View Menu\n2. New Order\n3. Sales Summary\n4. Exit\nChoice: ";
        if (!(cin >> choice)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); choice = 0; }
        switch (choice) {
            case 1: r.showMenu(); break;
            case 2: r.takeOrder(); break;
            case 3: r.showSales(); break;
            case 4: cout << "Thank you!\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 4);
    return 0;
}