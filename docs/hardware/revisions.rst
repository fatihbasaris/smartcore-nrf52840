Hardware Revisions
==================

Both revisions use the EBYTE E73-2G4M08S1C module and serve the same purpose.
The revisions include changes to pads beneath the module and the battery
charging circuitry.

+----------------------+------------------+------------------+
| Item                 | STICK V1         | STICK V2         |
+======================+==================+==================+
| Charging IC          | TP5400           | MCP73831         |
+----------------------+------------------+------------------+
| USB connector        | Mini USB         | Mini USB         |
+----------------------+------------------+------------------+
| Programming          | J-Link / SWD     | J-Link / SWD     |
+----------------------+------------------+------------------+

The schematics are the design references; additional functional verification
is required before claiming all peripherals have been tested.
