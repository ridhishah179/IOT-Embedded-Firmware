Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

**1. How much current does a single LED draw when the output drive is set to "Strong" with the original code?**

![](https://github.com/CU-ECEN-5823/ecen5823-assignments-ridhishah179/blob/master/Output_images/q1_strong_1.PNG)
![](https://github.com/CU-ECEN-5823/ecen5823-assignments-ridhishah179/blob/master/Output_images/q1_strong_2.PNG)

From the above Images the current drawn by the LED when output drive is set to strong is 
4.8 - 4.23 = 0.57mA

**2. After commenting out the standard output drive and uncommenting "Weak" drive, how much current does a single LED draw?**

![](https://github.com/CU-ECEN-5823/ecen5823-assignments-ridhishah179/blob/master/Output_images/q2_weak_1.PNG)
![](https://github.com/CU-ECEN-5823/ecen5823-assignments-ridhishah179/blob/master/Output_images/q2_weak_2.PNG)

From the above Images the current drawn by the LED when output drive is set to weak is 
4.78 - 4.22 = 0.56mA

**3. Is there a meaningful difference in current between the answers for question 1 and 2? Please explain your answer, 
referencing the [Mainboard Schematic](https://www.silabs.com/documents/public/schematic-files/WSTK-Main-BRD4001A-A01-schematic.pdf) and [AEM Accuracy](https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf) section of the user's guide where appropriate.**

The BlueGecko board outputs 3.3V as logic high. From the given schematic  we see that a 3.3k Resisitor is present. The forward voltage of LED is around 2 volts, therefore limiting current would be (3.3 - 2)/3.3k = 0.4mA
This current is less than the weak and strong strengths of the GPIO i.e 1mA and 10mA respectively. therfore, much difference is not observed between the currents.

**4. Using the Energy Profiler with "weak" drive LEDs, what is the average current and energy measured with only LED1 turning on in the main loop?**

![](https://github.com/CU-ECEN-5823/ecen5823-assignments-ridhishah179/blob/master/Output_images/q3_weak.PNG)

From the above Images the it is seen that the average current 4.59mA is and the Energy is 65.53uWh when the device ran for 15.49s

**5. Using the Energy Profiler with "weak" drive LEDs, what is the average current and energy measured with both LED1 and LED0 turning on in the main loop?**

![](https://github.com/CU-ECEN-5823/ecen5823-assignments-ridhishah179/blob/master/Output_images/q4_weak.PNG)

From the above Images the it is seen that the average current 4.76mA is and the Energy is 63.66uWh when the device ran for 14.2s

