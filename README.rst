.. _nrf_machine_learning_app:

Machine Learning demo
####################

.. contents::
   :local:
   :depth: 2

This demo is based on the nRF Machine Learning application, an out of the box reference design of an `embedded machine learning`_ using `Edge Impulse`_.
The application gathers data from sensors and runs the machine learning model.
It also displays results of the machine learning model on LEDs.
The Edge Impulse platform collects data from sensors, trains machine learning model, and deploys the model to your Nordic Semiconductor device.
To learn more about Edge Impulse support in the |NCS| see :ref:`ug_edge_impulse`.

Requirements
************

The application supports the following development kits:

.. table-from-sample-yaml::

The available configurations use only built-in sensors, and an external relay. 

.. include:: /includes/tfm.txt

The Thingy:53 should be mounted to a table fan and connected via the Qwiic-connector to a relay (https://www.sparkfun.com/products/15093) that intersects one of the mains-power wires of the fan. Use the Normally Open (NO) input of the relay.

Overview
********

To perform its tasks, the nRF Machine Learning application uses components available in Zephyr and the |NCS|, namely the :ref:`lib_caf` modules and :ref:`zephyr:sensor_api` for sampling sensors.

Sampling sensors
================

The application handles the sensor sampling using the :ref:`caf_sensor_manager`.
This module uses Zephyr's :ref:`zephyr:sensor_api` to handle the sampling.
This approach allows to use any sensor available in Zephyr.

By default, the following sensors are used by the application:

* Thingy:53 - Built-in accelerometer (``ADXL362``).

Machine learning model
======================

The application handles the machine learning model using the :ref:`ei_wrapper` library available in the |NCS|.
The model performs the classification task by assigning a label to input data.
The labels that are assigned by the machine learning model are specific to the given model.

By default, the application uses pretrained machine leaning models deployed in `Edge Impulse studio`_:

* Thingy:53 uses the `nRF Connect SDK hardware accelerometer machine learning model`_.
  The model uses the data from the built-in accelerometer to recognize the following vibration patterns:

  * ``speed_1`` - The fan is set to its lowes speed setting.
  * ``speed_2`` - The fan is set between its highest an lowes speed setting (optional).
  * ``speed_3`` - The fan is set to the its highest speed setting.
  * ``off`` - The fan is turned off. 

  Unknown vibration patterns, such as running the fan with weight added to one of the fan blades, are recognized as anomaly.

The application displays LED effects that correspond to the machine learning results.
If a large enough anomaly is detected, the Thingy:53 will try to send an I2C message to the connected relay to stop the fan. 
For more detailed information, see the `User interface`_ section.

Programming Thingy:53
=====================

|thingy53_sample_note|


Custom model requirements
=========================

The default application configurations rely on pretrained machine learning models that resides within the src folder under the name model.zip.
To get accurate results you need to train and deploy a custom machine learning model using `Edge Impulse Studio`_, you need a user account for the Edge Impulse Studio or our own Nordic ML Studio web-based tool.

The model needs to be trained with the labels "speed_1", "speed_2" (optional), "speed_3" and "off". 
This is done by first using the nRF Edge Impulse mobile app and firmware to collect and upload multiple samples with the fan set to each corresponding speed, of 5 seconds length per sample. Alternatively longer sample lenghts can be collected, and split up in the ML Studio platform.
Once the samples are collected and labeled and split 80/20 between training and testing, a model can be built using the following parameters: 

Impulse design: 
  * Window size: 5000 ms
  * Widnow increase: 5000 ms
  * Classification block
  * Anomaly detection block

Create impulse: 
  * Spectral features: 
  * Filter:
  * Scale axes: 1
  * Input decimation ratio: 1
  * Type: none
  * Analysis:
  * Type: wavelet
  * Wavelet decomposition ratio: 1
  * Wavelet: bior1.3

Classifier:
  * Number of training cycles: 40
  * Use learned optimizer: no 
  * Learning rate: 0.0005
  * Training processor: CPU
  * Validation set size: 20 %
  * Split train/validation set on metadata key: blank
  * Batch size: 32
  * Auto-weight classes: no
  * Profile int8 model: yes
  * Input layer (84 features)
  * Dense layer (40 neurons)
  * Dense layer (20 neurons)
  * Dropout (rate 0.25)
  * Output layer (3 classes)

Anomaly detection: 
  Select suggested axes 

Eon tuner: 
  Run eon tuner.

Configure your deployment: 
  Select C++ library, download the .zip file and replace model.zip in the src folder with your downloaded .zip C++ library. Then, rename your new .zip file to "model.zip". 


User interface
**************

The application supports a simple user interface.
You can control the application using predefined buttons, while LEDs are used to display information.

Buttons
=======

The application supports a button that is used to switch the fan on and off manually.

By default, the following buttons are used by the application:

* Thingy:53:

  * The **SW3** button starts the fan (activates the relay) on a single click and stops the fan (deactivates the relay) on a double click  

LEDs
====

The application uses one LED to display the application state.
The LED displays the machine learning prediction results.
You can configure the LED effect in the application configuration files.

By default, the application uses the following LED effects:

* Thingy:53 displays the application state in the RGB scale using **LED1**.

  * If the device is returning the machine learning prediction results, the LED uses following predefined colors:

    * ``speed_1`` - Blue
    * ``speed_2`` - Green
    * ``speed_3`` - Yellow
    * Anomaly - Blinking red

    If the machine learning model is running, but it has not detected anything yet or the ``off`` state is detected, the LED is blinking white.
    After a successful detection, the LED is set to the predefined color.
    The LED effect is overridden on the next successful detection.


Building and running
********************

.. |sample path| replace:: :file:`applications/machine_learning`

Build the application in VS Code and flash it to the Thingy:53 with the nRF Programmer PC app, using the dfu_application.zip from the build files. 

Testing
=======

Testing with Thingy:53
----------------------

After programming the application, perform the following steps to test the nRF Machine Learning application on the Thingy:

1. Turn on the Thingy.
    The application starts in a mode that runs the machine learning model.
    The RGB LED is blinking, because no gesture has been recognized by the machine learning model yet.
#. Click the Thingy:53's button to activate the relay to start the fan. 
    Notice that the color of the LED changes based on which speed setting the fan is set to. 
#. Double click to deactivate the relay to stop the fan.
    Useful for exiting uninterrupted normal state operation 
#. Add weight to the fan blades to create anomalies that will stop the fan automatically. 
    You might need to add more weight, or adjust the anomaly thresold values in the application to get reliable result. 
#. You can connect the Thingy:53 to a PC with a USB-cable and use a terminal monitor to readout the inferencing results. 
    Useful for when editing the value and anomaly thresholds.  


.. _nrf_machine_learning_app_internal_modules:

Application internal modules
****************************

The nRF Machine Learning application uses modules available in :ref:`lib_caf` (CAF), a set of generic modules based on Application Event Manager and available to all applications and a set of dedicated internal modules.
See `Firmware architecture`_ for more information.

The nRF Machine Learning application uses the following modules available in CAF:

* :ref:`caf_ble_adv`
* :ref:`caf_ble_bond`
* :ref:`caf_ble_smp`
* :ref:`caf_ble_state`
* :ref:`caf_buttons`
* :ref:`caf_click_detector`
* :ref:`caf_leds`
* :ref:`caf_power_manager`
* :ref:`caf_sensor_manager`

See the module pages for more information about the modules and their configuration.

The nRF Machine Learning application also uses the following dedicated application modules:

``ei_data_forwarder_bt_nus``
  Not in use for this demo application

``ei_data_forwarder_uart``
  The module forwards the sensor readouts over UART.

``led_state``
  This is where you edit the value and anomaly thresolds.  
  The module displays the application state using LEDs.
  The LED effects used to display the state of data forwarding, the machine learning results, and the state of the simulated signal are defined in :file:`led_state_def.h` file located in the application configuration directory.
  The common LED effects are used to represent the machine learning results and the simulated sensor signal.

``ml_runner``
  The module uses :ref:`ei_wrapper` API to control running the machine learning model.
  It provides the prediction results using :c:struct:`ml_result_event`.

``ml_app_mode``
  Not in use for this demo application

``sensor_sim_ctrl``
  Not in use for this demo application

``usb_state``
  The module enables USB.

Dependencies
************

The application uses the following Zephyr drivers and libraries:

* :ref:`zephyr:sensor_api`
* :ref:`zephyr:uart_api`
* :ref:`zephyr:mcu_mgr`

The application uses the following |NCS| libraries and drivers:

* :ref:`app_event_manager`
* :ref:`lib_caf`
* :ref:`ei_wrapper`
* :ref:`nus_service_readme`

The sample also uses the following secure firmware component:

* :ref:`Trusted Firmware-M <ug_tfm>`

In addition, you can use the :ref:`central_uart` sample together with the application.
The sample is used to receive data over NUS and forward it to the host over UART.

API documentation
*****************

Following are the API elements used by the application.

Edge Impulse Data Forwarder Event
=================================

| Header file: :file:`applications/machine_learning/src/events/ei_data_forwarder_event.h`
| Source file: :file:`applications/machine_learning/src/events/ei_data_forwarder_event.c`

.. doxygengroup:: ei_data_forwarder_event
   :project: nrf
   :members:

Machine Learning Application Mode Event
=======================================

| Header file: :file:`applications/machine_learning/src/events/ml_app_mode_event.h`
| Source file: :file:`applications/machine_learning/src/events/ml_app_mode_event.c`

.. doxygengroup:: ml_app_mode_event
   :project: nrf
   :members:

Machine Learning Result Event
=============================

| Header file: :file:`applications/machine_learning/src/events/ml_result_event.h`
| Source file: :file:`applications/machine_learning/src/events/ml_result_event.c`

.. doxygengroup:: ml_result_event
   :project: nrf
   :members:

Sensor Simulator Event
======================

| Header file: :file:`applications/machine_learning/src/events/sensor_sim_event.h`
| Source file: :file:`applications/machine_learning/src/events/sensor_sim_event.c`

.. doxygengroup:: sensor_sim_event
   :project: nrf
   :members:
