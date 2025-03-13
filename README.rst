
BTS7960 Pin	nRF5340 Pin	Function
RPWM	P0.18	Motor PWM (Right)
LPWM	P0.19	Motor PWM (Left)
R_EN	P0.16	Enable Right Channel
L_EN	P0.17	Enable Left Channel
R_IS	NC	Not Used (Overcurrent Protection)
L_IS	NC	Not Used (Overcurrent Protection)
VCC	3.3V	Logic Power
GND	GND	Ground
M+	Motor Terminal 1	Motor Output
M-	Motor Terminal 2	Motor Output
B+	Battery Positive (12V or 24V)	Motor Power
B-	Battery Ground	Motor Power
 Functionality:

RPWM and LPWM are controlled using PWM signals to adjust speed and direction.
R_EN and L_EN must be set HIGH to enable movement.
BTS7960 provides high-power motor control (supports up to 43A).


Encoder Pin	nRF5340 Pin	Function
Encoder PWR+	3.3V	Encoder Power
Encoder PWR-	GND	Ground
MOTOR PWR+	BTS7960 M+	Motor Terminal 1
MOTOR PWR-	BTS7960 M-	Motor Terminal 2
OUT C1/A	P0.14	Quadrature Encoder Signal A
OUT C2/B	P0.15	Quadrature Encoder Signal B
✅ Functionality:

OUT C1/A and OUT C2/B provide quadrature signals to track motor position & speed.
These pins are read using interrupts or timers in nRF5340.

.. zephyr:code-sample:: blinky
   :name: Blinky
   :relevant-api: gpio_interface

   Blink an LED forever using the GPIO API.

Overview
********

The Blinky sample blinks an LED forever using the :ref:`GPIO API <gpio_api>`.

The source code shows how to:

#. Get a pin specification from the :ref:`devicetree <dt-guide>` as a
   :c:struct:`gpio_dt_spec`
#. Configure the GPIO pin as an output
#. Toggle the pin forever

See :zephyr:code-sample:`pwm-blinky` for a similar sample that uses the PWM API instead.

.. _blinky-sample-requirements:

Requirements
************

Your board must:

#. Have an LED connected via a GPIO pin (these are called "User LEDs" on many of
   Zephyr's :ref:`boards`).
#. Have the LED configured using the ``led0`` devicetree alias.

Building and Running
********************

Build and flash Blinky as follows, changing ``reel_board`` for your board:

.. zephyr-app-commands::
   :zephyr-app: samples/basic/blinky
   :board: reel_board
   :goals: build flash
   :compact:

After flashing, the LED starts to blink and messages with the current LED state
are printed on the console. If a runtime error occurs, the sample exits without
printing to the console.

Build errors
************

You will see a build error at the source code line defining the ``struct
gpio_dt_spec led`` variable if you try to build Blinky for an unsupported
board.

On GCC-based toolchains, the error looks like this:

.. code-block:: none

   error: '__device_dts_ord_DT_N_ALIAS_led_P_gpios_IDX_0_PH_ORD' undeclared here (not in a function)

Adding board support
********************

To add support for your board, add something like this to your devicetree:

.. code-block:: DTS

   / {
   	aliases {
   		led0 = &myled0;
   	};

   	leds {
   		compatible = "gpio-leds";
   		myled0: led_0 {
   			gpios = <&gpio0 13 GPIO_ACTIVE_LOW>;
                };
   	};
   };

The above sets your board's ``led0`` alias to use pin 13 on GPIO controller
``gpio0``. The pin flags :c:macro:`GPIO_ACTIVE_HIGH` mean the LED is on when
the pin is set to its high state, and off when the pin is in its low state.

Tips:

- See :dtcompatible:`gpio-leds` for more information on defining GPIO-based LEDs
  in devicetree.

- If you're not sure what to do, check the devicetrees for supported boards which
  use the same SoC as your target. See :ref:`get-devicetree-outputs` for details.

- See :zephyr_file:`include/zephyr/dt-bindings/gpio/gpio.h` for the flags you can use
  in devicetree.

- If the LED is built in to your board hardware, the alias should be defined in
  your :ref:`BOARD.dts file <devicetree-in-out-files>`. Otherwise, you can
  define one in a :ref:`devicetree overlay <set-devicetree-overlays>`.


