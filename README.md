BitBank temperature/humidity/pressure sensor Library<br>
----------------------------------------------------
Copyright (c) 2023 BitBank Software, Inc.<br>
Written by Larry Bank<br>
email: bitbank@pobox.com<br>
<br>
<b>What does it do?</b><br>
The library auto-detects a growing list of popular I2C environmental sensors. It provides a consistent API across all of the different devices and allows capabilities to be queried at runtime.<br>
<b>Why did you write it?</b><br>
Lately I've been on a mission to simplify the use of I2C sensors for myself and others. One way to make it easier is to support an entire category of devices with a single library. Another is to auto-detect the device and its I2C address automatically. A recent experience is one of the reasons I wrote this code - when working with a colleague on a T/H/P sensor project, I had gotten things working and with the same software and hardware, his didn't. It turned out that his sensor breakout board had configured an alternal I2C address and the software he used to communicate with it failed to 'see' the sensor at the alternate address. This is a simple problem to solve with I2C devices since the address bus can be queried to see if a device responds at a specific address. The solution is simply to 'ping' multiple possible addresses to find the available devices. Personally, I'm constantly testing different sensors from different vendors and this library allows me to swap parts without changing my code.<br>
<br>


If you find this code useful, please consider becoming a sponsor or sending a donation.

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=SR4F44J2UR8S4)


