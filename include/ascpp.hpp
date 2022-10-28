#pragma once

class App {
 public:
  App();
  App(App&&) = default;
  App(const App&) = default;
  App& operator=(App&&) = default;
  App& operator=(const App&) = default;
  ~App();

 private:
};
