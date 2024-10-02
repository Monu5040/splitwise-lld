#include<bits/stdc++.h>
using namespace std;

enum class ExpenseType {
    EQUAL,
    EXACT,
    PERCENTAGE
};

class User {
public:
    string user_id;
    string name;
    string email_id;
    int number;
    int total_owed;
    int total_borrowed;
    unordered_map<shared_ptr<User>,int>owed_user;
    unordered_map<shared_ptr<User>,int>borrowed_user;
    User(string user_id, int total_owed, int total_borrowed) :
    user_id(user_id), total_owed(total_owed), total_borrowed(total_borrowed) {}

    void addOwedUser(shared_ptr<User> payer, int amount) {
        owed_user[payer]+=amount;
    }

    void addBorrowedUser(shared_ptr<User> payer, int amount) {
        borrowed_user[payer]+=amount;
    }
};

class ExpenseStartegy {
public:
    virtual void splitExpense(shared_ptr<User> payer,int amount, unordered_map<shared_ptr<User>,int>&borrower) = 0;
};

class EqualExpenseStrategy : public ExpenseStartegy {
public:
    EqualExpenseStrategy() {
        cout<<"EqualExpenseStrategy object created "<<endl;
    }
    void splitExpense(shared_ptr<User> payer,int amount, unordered_map<shared_ptr<User>,int>&borrower) override {
        cout<<"Equal expense startegy"<<endl;
        int sz = borrower.size();
        int due = amount/sz;
        for(auto item : borrower) {
            shared_ptr<User>borrower_user = item.first;
            borrower_user->addOwedUser(payer, due);
            payer->addBorrowedUser(borrower_user,due);
        }
        payer->total_owed += amount;

    }
};

class ExactExpenseStrategy : public ExpenseStartegy {
public:
    void splitExpense(shared_ptr<User> payer,int amount, unordered_map<shared_ptr<User>,int>&borrower) override {
        cout<<"Exact expense startegy"<<endl;
        for(auto item : borrower) {
            shared_ptr<User>borrower_user = item.first;
            borrower_user->addOwedUser(payer, item.second);
            payer->addBorrowedUser(borrower_user,item.second);
        }
        // payer->total_owed += amount;
    }
};

class PercentageExpenseStrategy : public ExpenseStartegy {
public:
    void splitExpense(shared_ptr<User> payer,int amount, unordered_map<shared_ptr<User>,int>&borrower) override {
        cout<<"Percentage expense startegy"<<endl;
        for(auto item : borrower) {
            int due = amount*(item.second/100);
            shared_ptr<User>borrower_user = item.first;
            borrower_user->addOwedUser(payer, due);
            payer->addBorrowedUser(borrower_user,due);
        }
    }
};

class ExpenseFactory {
public:
    shared_ptr<ExpenseStartegy> getExpenseStrategyObject(ExpenseType type) {
        switch(type) {
            case ExpenseType::EQUAL:
                return make_shared<EqualExpenseStrategy>();
            case ExpenseType::EXACT:
                return make_shared<ExactExpenseStrategy>();
            case ExpenseType::PERCENTAGE:
                return make_shared<PercentageExpenseStrategy>();
            default:
                return nullptr;
        }
    }
};

class UserManager {
private:
    unordered_map<string,shared_ptr<User>>users;
public:
    void addUser(string user_id, shared_ptr<User>user) {
        if(users.find(user_id) != users.end()) {
            cout<<"user already exists"<<endl;
            return;
        }
        users[user_id] = user;
    }

    void removeUser(string user_id) {
        //remove user
    }

    unordered_map<string,shared_ptr<User>> getUsers() {return users;}
};


class Split {
private:
    shared_ptr<UserManager>user_manager;
    unordered_map<string,shared_ptr<User>>users;
public:
    Split(shared_ptr<UserManager>user_manager) : user_manager(user_manager) {
        users = user_manager->getUsers();
    }

    void addExpense(string payer, int amount, unordered_map<string,int>&borrower, ExpenseType type) {
        
        if(users.find(payer) == users.end()) {
            cout<<"Payer doesnot exists"<<endl;
            return;
        }
        shared_ptr<User>payer_obj = users[payer];
        unordered_map<shared_ptr<User>,int>borrower_obj_mp;
        for(auto item : borrower) {
            if(users.find(item.first) != users.end()) {
                shared_ptr<User>user = users[item.first];
                borrower_obj_mp[user] = item.second;
            }
        }
        shared_ptr<ExpenseFactory>factory_obj = make_shared<ExpenseFactory>();
        shared_ptr<ExpenseStartegy> startegy_obj = factory_obj->getExpenseStrategyObject(type);
        
        startegy_obj->splitExpense(payer_obj,amount,borrower_obj_mp);
    }

    void shopOwedData(string user) {
        if(users.find(user) == users.end()) {
            cout<<user<<" does not exists"<<endl;
        }
        shared_ptr<User>user_obj = users[user];
        unordered_map<shared_ptr<User>,int> ower_list = user_obj->owed_user;
        if(ower_list.size()) {
            cout<<user;
            for(auto ower : ower_list) {
                cout<<" owes "<<ower.first->user_id<<" : "<<ower.second<<endl;
            }
        }
    }
    

};

int main() {
    shared_ptr<User>user1 = make_shared<User>("Monu",0,0);
    shared_ptr<User>user2 = make_shared<User>("Sonu",0,0);
    shared_ptr<User>user3 = make_shared<User>("Raja",0,0);
    shared_ptr<User>user4 = make_shared<User>("Saskhi",0,0);
    shared_ptr<User>user5 = make_shared<User>("Romi",0,0);
    shared_ptr<UserManager>user_manager = make_shared<UserManager>();
    user_manager->addUser("Monu", user1);
    user_manager->addUser("Sonu", user2);
    user_manager->addUser("Raja", user3);
    user_manager->addUser("Saskhi", user4);
    user_manager->addUser("Romi", user5);
    unordered_map<string,shared_ptr<User>>users = user_manager->getUsers();
    // for(auto item:users){
    //     cout<<item.first<<item.second->user_id<<endl;
    // }

    shared_ptr<Split>split_app = make_shared<Split>(user_manager);
    unordered_map<string,int>mp = {{"Sonu",0},{"Romi",0}};
    split_app->addExpense("Monu",20000,mp,ExpenseType::EQUAL);
    split_app->shopOwedData("Romi");
    mp = {{"Sonu",700},{"Romi",300}};
    split_app->addExpense("Monu",1000,mp,ExpenseType::EXACT);
    split_app->shopOwedData("Romi");
}