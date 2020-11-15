# domestic-supervisor

[![GitHub license](https://img.shields.io/github/license/LuigiPiucco/domestic-supervisor?color=informational)][2] [![built with nix](https://builtwithnix.org/badge.svg)][1]

A toy smart home simulator project.

<a href="./README.pt.md">
<img src="https://upload.wikimedia.org/wikipedia/commons/3/31/Flag_of_Portuguese_language_%28PT-BR%29.svg" alt="Português" title="Ler em português" width="32px" />
</a>
<a href="./README.ja.md">
<img src="https://upload.wikimedia.org/wikipedia/en/thumb/9/9e/Flag_of_Japan.svg/1920px-Flag_of_Japan.svg.png" alt="日本語" title="日本語で読みます" width="32px" />
</a>

**Table of Contents**

- [domestic-supervisor](#domestic-supervisor)
  - [Project](#project)
  - [Interface design](#interface-design)
  - [Implementation](#implementation)
    - [Common entities](#common-entities)
    - [Supervisor entities](#supervisor-entities)
    - [Simulator entities](#simulator-entities)
  - [Acknowledgements](#acknowledgements)

## Project

Entails creating two programs, one to serve as a supervisor to a smart home's
systems and another to simulate said systems of this house, as no test bench is
available.

The programs should communicate through MQTT, a protocol used largely in
domestic automation.

The controlled/simulated systems are:

- Face detecting camera;
- Remote locks;
- Remote light bulbs;

Supervisor features:

- Show the camera's image with a marking of where (if present) a face was detected;
- Show the house's blue print with the indication of the lights and locks, which
  can be clicked to toggle state;
- React to events sent by the simulator, such as state changes to lights or
  locks;

Simulator features:

- Capture and image from the computer's webcam, sending it properly through
  MQTT;
- React to events sent by the supervisor, such as state changes to lights or
  locks, printing it in its view and returning a success signal to the
  supervisor;
- Simulate toggling lights or locks, at the physical location, external to the
  system, by showing a blueprint of the house and allowing clicks to toggle
  state;

## Interface design

Camera view:

![Camera view](./assets/Camera.png)

Blue print view:

![Blue print view](./assets/Blueprint.png)

## Implementation

The project's implementation was done in C++, in accordance to modern C++
good code practices.

### Common entities

- `utils` namespace:
  - Multiple `event`:
    - Keeps track of functions to be called upon being triggered;
    - May be called as a function to trigger;
    - Intended to be owned by some other object that has events as a concept
      and exposed by reference so callbacks may be registered;
- `controllers` namespace:
  - 2 `state`:
    - Manages the residence's state, keeping tabs of which lights and locks are
      on;
    - Registers callbacks to do things when the lights and/or locks change;
  - 2 `mqtt` that inherits from `mqtt::callback` and `mqtt::iaction_listener`:
    - Responsible for processing incoming messages and providing an API to send
      messages to the other aplication;
- Multiple `device`:
  - Represents a single device, capable of being toggle on or off;
  - Pairs this state information with constant location, type (light or lock),
    description and name information;
- `view` interface that inherits from `Wt::WContainerWidget`:
  - Represents a view of the application;
  - Has a name and a path plus the elements of `Wt::WContainerWidget`;
- Entities that inherit from `view` in the `views` namespace:
  - 2 `blueprint`:
    - The blueprint views's root widget;
    - Shown when the user navigates to "/blueprint";

### Supervisor entities

- `controllers` namespace:
  - 1 `deepnet`:
    - Responsible for applying the face detection algorithm on received images
      and marking the detections;
    - Processes in a separate thread, as to not block the GUI with a heavy image
      processing workload;
    - Allows to push images to a queue, which will be processed in FIFO order;
    - Does its processing through a deep neural network built with the dlib
      library;
    - May be disabled/enabled, to mitigate resource usage when the camera view
      is not shown or the user does not want facial detection;
    - Registers callbacks to do things when the number of detections changes;
- Entities that inherit from `view` in the `views` namespace:
  - 1 `camera`:
    - The camera view's root widget;
    - Shown when the user navigates to the path "/camera" or "/";
- Entities that inherit from a descendant of `Wt::WWidget` in the `widgets` 
  namespace:
  - 1 `camera_panel`:
    - Draws the camera control panel according to the detection state and
      updates the count when it changes;
  - 2 `page_anchor`:
    - Draws the page switch button by receiving the arguments
      - Entity that implements `view` to be shown when clicked;
- 1 `dynamic_image` that inherits from `Wt::WResource`:
  - Provides a way to send a dynamic image to the user, for showing the camera;
- 1 `domestic_supervisor` that inherits from `Wt::WApplication`:
  - Represents the application as a whole;
  - Defines the applications's life-cycle methods (startup, connect, run,
    shutdown...);
- 1 `domestic_server` that inherits from `Wt::WServer`:
  - Configures the server to send the application under multiple URLs;
  - Creates `domestic_supervisor` sessions on demand, so, in practice, there are
    many of every entity, but for simplicity, only the number is a single
    session is counted in this listing;

### Simulator entities

- `controllers` namespace:
  - 1 `image_loop`:
    - Collects and sends webcam images through MQTT;
    - Runs in loop on separate, self managed thread;
    - Can be called as a function;
- 1 `domestic_simulator` that inherits from `Wt::WApplication`:
  - Represents the application as a whole;
  - Defines the application's life-cycle methods (startup, connect, run,
    shutdown...);

## Acknowledgments

Light bulb icons made by
[Good Ware](https://www.flaticon.com/authors/good-ware "Good Ware") from
[https://www.flaticon.com](https://www.flaticon.com "Flaticon").

Padlock icons made by Fabián Alexis,
[CC BY-SA 3.0](https://creativecommons.org/licenses/by-sa/3.0 "CC BY-SA 3.0"),
via Wikimedia Commons

Note about CC BY-SA: As I understand it, since the icons under CC are only
collected, not modified in any way, I'm allowed to license my own work under a
different license.

[1]: https://builtwithnix.org "Built with nix"
[2]: https://github.com/LuigiPiucco/domestic-supervisor/blob/master/LICENSE "Github license"
