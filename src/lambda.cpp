#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>

class session_t
{
public:
    std::ostream &operator<<(std::ostream &os)
    {
        os << m_name;
        return os;
    }

private:
    std::string m_name = "socket";
};

int main()
{
    std::unique_ptr<session_t> session;
    auto print = [session = std::move(session), time = std::chrono::system_clock::now()]() {
        std::cout << std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count()
                  << session << std::endl;
    };
    std::cout << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    print();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    print();
}