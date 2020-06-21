# RPM_Meter_Direction_V3

https://youtu.be/lOM5GylRBGY

RPM meter with quadrature encoder using Hall effect sensors.

I use KY-035 sensors that have an AH49E analog sensor.  For each motor I have placed two mechanically separated sensors in quadrature to determine the direction of rotation.

With the use of external interruptions I control the state of sensors 1 of each pair and with internal interruptions (Timer1) I count the time of the period to determine the frequency and the RPM.  It used exponential decay by not detecting changes in sensor 1 of each encoder, either because the speed has dropped considerably or the motor has stopped.

On each axis of the motors I have improvised an encoder with neodymium magnets.

For starting and turning control of each motor, I use operational amplifiers (LM324N) with a joystick and a driver (L293N and L298N).

Code: https://github.com/DrakerDG/RPM_Meter_Direction_V3

Reference:
https://youtu.be/BAfxA7JvHsE
https://youtu.be/epU-XlbErkY
https://youtu.be/dTuYLGqXNRg
https://youtu.be/QZ8umuWt5Zw
https://youtu.be/iQrE2IDGjI8
https://youtu.be/oSBpj-XV1_o
