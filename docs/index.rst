SmartCore nRF52840 Documentation
================================

**Hardware Reference & Developer Guide**

Welcome to the documentation for **SmartCore nRF52840**, a family of custom
nRF52840 development boards built around the **EBYTE E73-2G4M08S1C** module.
These boards are intended for embedded firmware development, peripheral
interfacing, and wireless application prototyping.

This documentation covers two hardware revisions: **STICK V1** and **STICK V2**.
Both revisions use the same radio module, but their PCB layouts and power
circuits differ. Always consult the documentation for your specific revision
before connecting external hardware.

Key Features
------------

* Nordic nRF52840-based EBYTE E73-2G4M08S1C module
* 3.3 V regulated supply circuitry and Mini USB connector
* SWD programming and debugging through SEGGER J-Link
* Two user LEDs and a user button
* GPIO expansion and interfaces including UART and I2C
* Battery-related circuitry specific to each hardware revision
* Firmware development using nRF Connect SDK and Zephyr RTOS

Hardware Revisions
------------------

.. list-table:: Hardware revision overview
   :header-rows: 1
   :widths: 25 35 40

   * - Item
     - STICK V1
     - STICK V2
   * - Core module
     - E73-2G4M08S1C
     - E73-2G4M08S1C
   * - Battery circuitry
     - TP5400-based circuit
     - MCP73831-based charging circuit
   * - PCB design
     - Initial revision
     - Revised module-under-pad layout
   * - Programming interface
     - SWD / J-Link
     - SWD / J-Link

See :doc:`hardware/revisions` for details and the revision-specific pages
:doc:`hardware/stick-v1` and :doc:`hardware/stick-v2`.

Development Environment
-----------------------

The initial GPIO test was performed with **Visual Studio Code**, **nRF Connect
SDK**, and a **SEGGER J-Link** programmer. The firmware was built for
``nrf52840dk/nrf52840`` and used **P1.06** to blink an onboard LED.

.. note::

   ``nrf52840dk/nrf52840`` is a Nordic development-kit target used for the
   initial test; it is not a dedicated SmartCore board definition. Other
   peripherals may need custom Devicetree or board configuration.

Getting Started
---------------

Begin with :doc:`getting-started/index` for the development setup and the
first firmware example. Browse :doc:`hardware/index` for revision-specific
hardware information and :doc:`examples/index` for application examples.

Documentation Contents
----------------------

.. toctree::
   :maxdepth: 2
   :caption: Contents

   getting-started/index
   hardware/index
   software/index
   examples/index
   resources/index

