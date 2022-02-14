# chhavi_esphome
esphome repo for chhavi ( Wireless fingerprint and nfc sensor )
# ESPHome custom component examples

This repository provides examples PN7150 component that can be used as templates to quickly develop your own PN7150 nfc component for the splendid [ESPHome](https://esphome.io/) ESP8266/ESP32 home automation system.

## How to use
All sample components can be found in the `custom_components` directory. The `test.yaml` file provides configuration examples for PN7150 component. To use a component for your project, do the following:

- Create a `custom_components` directory in your esphome configuration directory (the directory where your ```.yaml``` files are)

- Copy the directory of an PN7150 component to `custom_components` in its entirety, so you end up with e.g. `custom_components/pn7150/`

- Find the configuration entry for the PN7150 component in `test.yaml` and copy it into your own `.yaml` file.

- Compile with `esphome your_config.yaml compile` (change `your_config.yaml` to your own `.yaml` file) or compile with the dashboard.

- No errors? Great! You can now start using it.

## Basic structure of a PN7150 component

Let's start with the simplest custom component:

```
custom_components
├── pn7150
│   ├── __init.py__
│   ├── binary_sensor.py
│   ├── PN7150.cpp
│   ├── PN7150.h
│  ...
```

The ```__init.py__``` file contains 2 main things: 
- **configuration validation** or **cv** 
- **code generation** or **cg**

**cv** handles validation of user input in the `.yaml` configuration file for this particular component: defining the available configuration options, whether they are _required_ or _optional_, any constraints on the types and values of an option, etc.

**cg** takes these validated configuration options and generates the code necessary to streamline them into your c++ code, as well as registering your component properly within the ESPHome runtime.

The ```.cpp``` and ```.h``` files are the main source code files for your component. You're free to add to or modify the structure of the source code within the constraints of `c++`, but your code needs at least one `class` derived from `Component` that will be registered in the `__init__.py`.

If your component yields a specific type of component, e.g. a `sensor` or a `switch`, ESPHome will instead look for a `sensor.py` or `switch.py`.
The structure of these is identical to that of `__init__.py`.