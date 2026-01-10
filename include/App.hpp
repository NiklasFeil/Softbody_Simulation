#include "WindowManager.hpp"
#include <memory>

class App {
  private:
    std::unique_ptr<WindowManager> m_window_manager; 

  public:
    App();
    ~App();

    void run();
};