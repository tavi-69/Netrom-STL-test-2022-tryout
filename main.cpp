#include <bits/stdc++.h>
#define FAST ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
#define LSOne(s) ((s) & -(s))
typedef long long ll;
using namespace std;

struct person {
    string name;
    ll age, no_topics;
    mutable ll pritority;

    bool operator <(const person& other) const {
        if (pritority != other.pritority) return pritority < other.pritority;
        if (name != other.name) return name < other.name;
        if (age != other.age) return age < other.age;
        return no_topics < other.no_topics;
    }
};

ifstream InFile("input3.txt");
map<person, vector<string>> list_of_persons;
map<string, vector<string>> topics;

void read_clients() {
    ll size;
    InFile >> size;
    for (ll i = 0; i < size; ++i) {
        string name;
        ll age, no_topics;
        InFile >> name >> age >> no_topics;

        for (ll j = 0; j < no_topics; j++) {
            string topic;
            InFile >> topic;

            list_of_persons[{name, age, no_topics, 1}].push_back(topic);
        }
    }
}

void read_topics() {
    ll size;
    InFile >> size;
    for (ll i = 0; i < size; ++i) {
        string topic; // key
        ll no_topics; // size

        InFile >> topic >> no_topics;
        for (ll j = 0; j < no_topics; j++) {
            string subtopics;
            InFile >> subtopics;
            topics[topic].push_back(subtopics);
        }

    }
}

void find_all_topic_related(set<string>& all_topics, string topic) {
    // find the father of the node if it exists
    const auto it = find_if(begin(topics), end(topics),
        [&topic](const pair<string, vector<string>>& other) {
            for (const auto& index : other.second) {
                if (index == topic) return true;
            }
            return false;
        }
    );

    all_topics.insert(topic);
    if (it != topics.end()) find_all_topic_related(all_topics, it->first);

}

void interested_topics (string name) {
    // find the key
    const auto it = find_if(begin(list_of_persons), end(list_of_persons),
        [&name](const pair<person, vector<string>>& other) {
        return other.first.name == name;
    });

    if (it != list_of_persons.end()) {
        const person key = it->first;
        // use a set so it sorts them lexicographically and does not keep duplicates
        set<string> all_topics;

        for (const auto& index : list_of_persons[key]) {
            find_all_topic_related(all_topics, index);
        }

        cout << all_topics.size() << " ";
        for (const auto& topic : all_topics) {
            cout << topic << " ";
        }
        cout << endl;
    }
}

void find_customers(vector<person>& perfect_customers, string topic, ll pro) {
    if (topic == "-1") return; // no topic so it returns
    for (auto& it : list_of_persons) {
        // find each person who is interested in this topic
        auto index = find_if(begin(it.second), end(it.second),
            [&topic](const string& other) {
                return other == topic;
            });
        if (index != it.second.end()) {
            // check if duplicate
            auto see_if_is_already = find_if(begin(perfect_customers), end(perfect_customers), [&it](const person& other) {
                return it.first.name == other.name;
            });
            if (see_if_is_already == perfect_customers.end()) {
                it.first.pritority += pro;
                perfect_customers.push_back(it.first);
            }
        }
    }
}

void remove_person(ll age, vector<person>& perfect_customers) {
    ll mx = -1;
    ll prio = -1;
    person remover = {};
    for (const auto& it : perfect_customers) {
        // first by priority
        if (it.pritority > prio) {
            mx = abs(age - it.age);
            prio = it.pritority;
            remover = it;
        }
        else if (it.pritority == prio) {
            // second by how close to the ad age
            if (abs(age - it.age) > mx) {
                mx = abs(age - it.age);
                prio = it.pritority;
                remover = it;
            }
            else if (abs(age - it.age) == mx) {
                // third lexicographically
                if (it.name > remover.name) {
                    mx = abs(age - it.age);
                    prio = it.pritority;
                    remover = it;
                }
            }
        }
    }

    // find the index of the person to remove
    const auto index = find_if(perfect_customers.begin(), perfect_customers.end(),
        [&remover](const person& other) {
            if (remover.name == other.name and remover.no_topics == other.no_topics and remover.age == other.age) return true;
            return false;
        });
    perfect_customers.erase(index);
}

string next_topic(string topic) {
    // search in the map if there is the topic
    const auto it = find_if(begin(topics), end(topics),
        [&topic](const pair<string, vector<string>>& other) {
            for (const auto& index : other.second) {
                if (index == topic) return true;
            }
            return false;
        }
    );

    // returns the key where it found the topic
    if (it != topics.end()) {
        return it->first;
    }

    // else returns the topic
    return topic;
}

bool no_minuses (vector<string> topics_to_find) {
    ll cnt = 0;
    for (const auto it : topics_to_find) {
        if (it == "-1") cnt++;
    }
    if (cnt == topics_to_find.size()) return false;
    return true;
}

void find_all_customers_for_ad(vector<string> topics_to_find,ll no_persons_to_find, ll age_intended) {
    ll ok = 1; // the priority increment for each node
    vector<person> perfect_customers;
    for (const auto& topic : topics_to_find) {
        find_customers(perfect_customers, topic, ok);
    }

    // only need to remove
    if (perfect_customers.size() > no_persons_to_find) {
        while (perfect_customers.size() != no_persons_to_find) {
            remove_person(age_intended, perfect_customers);
        }
        cout << perfect_customers.size() << " ";
        for (const auto& customer : perfect_customers) {
            cout << customer.name << " ";
        }
        cout << endl;
    }
    // i have more budget so i try to find more people close to the topic if it exists
    else if (perfect_customers.size() < no_persons_to_find and no_minuses(topics_to_find)) {
        while (perfect_customers.size() < no_persons_to_find and no_minuses(topics_to_find)) {
            for (auto& it : topics_to_find) {
                if (it == next_topic(it)) {
                    it = "-1"; // it was the parent node
                }
                else {
                    it = next_topic(it);
                    ok++; // increase priority to sort them
                }
            }

            for (const auto& topic : topics_to_find) {
                find_customers(perfect_customers, topic, ok);
            }

        }

        // only need to remove
        if (perfect_customers.size() > no_persons_to_find and no_minuses(topics_to_find)) {
            while (perfect_customers.size() != no_persons_to_find and no_minuses(topics_to_find)) {
                remove_person(age_intended, perfect_customers);
            }
        }

        // sort them by priority else by how close to the ad age they are else lexicographically
        sort(perfect_customers.begin(), perfect_customers.end(), [&age_intended](const person& a, const person& b) {
            if (a.pritority != b.pritority) return a.pritority < b.pritority;
            if (abs(age_intended - a.age) != abs(age_intended - b.age)) return abs(age_intended - a.age) < abs(age_intended - b.age);
            return a.name < b.name;
        });
        cout << perfect_customers.size() << " ";
        for (const auto& customer : perfect_customers) {
            cout << customer.name << " ";
        }
        cout << endl;
    }
}


int main() {
    FAST
    read_clients();
    read_topics();

    // read the person to output topics he is interested in
    string person_to_find_topics;
    InFile >> person_to_find_topics;

    interested_topics(person_to_find_topics);

    // read the persons to output the persons i need to advertise ad to
    ll no_persons_to_find, age_intended, no_topics_to_find;
    InFile >> no_persons_to_find >> age_intended >> no_topics_to_find;

    // store the topics in a vector
    vector<string> topics_to_find(no_topics_to_find);
    for (ll i = 0; i < no_topics_to_find; ++i) InFile >> topics_to_find[i];

    find_all_customers_for_ad(topics_to_find, no_persons_to_find, age_intended);
    return 0;
}
