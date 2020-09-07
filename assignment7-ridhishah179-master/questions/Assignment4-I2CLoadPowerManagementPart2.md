Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

*Be sure to take measurements in the "Default" configuration of the profiler to ensure your logging logic is not impacting current/time measurements.*

*Please include screenshots of the profiler window detailing each current measurement captured.  See [Shared document](https://docs.google.com/document/d/1Ro9G2Nsr_ZXDhBYJ6YyF9CPivb--6UjhHRmVhDGySag/edit?usp=sharing) for instructions.* 

1. What is the average current per period?
   
   Answer: 68.24uA
   <br>Screenshot:  
   ![Avg_current_per_period](https://github.com/CU-ECEN-5823/ecen5823-assignment4-ridhishah179/blob/master/questions/screenshots/ss_question1_assignment4.PNG)  

2. What is the average current when the Si7021 is Load Power Management OFF?
   
    Answer: 3.43uA
   <br>Screenshot:  
   ![Avg_current_LPM_Off](https://github.com/CU-ECEN-5823/ecen5823-assignment4-ridhishah179/blob/master/questions/screenshots/ss_question2_assignment4.PNG)  

3. What is the average current when the Si7021 is Load Power Management ON?
   
   Answer: 1.47mA 
   <br>Screenshot: 
   ![Avg_current_LPM_Off](https://github.com/CU-ECEN-5823/ecen5823-assignment4-ridhishah179/blob/master/questions/screenshots/ss_question3_assignment4.PNG)  


4. How long is the Si7021 Load Power Management ON for 1 temperature reading?
   
   Answer: 125ms
   <br>Screenshot: 
   ![duration_lpm_on](https://github.com/CU-ECEN-5823/ecen5823-assignment4-ridhishah179/blob/master/questions/screenshots/ss_question4_assignment4.PNG)  

5. What is the total operating time of your design for assignment 4 in hours assuming a 1000mAh supply?
   
   Total Operating Time = ((power supply)/average current for a period * period = (1000/(68.24uA*1000mAh)*3s)/3600 = 13.5 hours

6. How has the power consumption performance of your design changed since the previous assignment?
   
   Since I2C implementation is interrupt based,the power consumption has reduced in this assignment.
   The power consumption in assignment 3 was found to be 463.70uW whereas it is 231.5uW in assignment 4. it has reduced nearly by 50%

7. Describe how you have tested your code to ensure you are sleeping in EM1 mode during I2C transfers.

   ![duration_lpm_on](https://github.com/CU-ECEN-5823/ecen5823-assignment4-ridhishah179/blob/master/questions/screenshots/ss_question7_assignment4.PNG) 
   From the screenshot we can see that average i2c read write current is 3.47 mA which shows that it is sleeping in EM1
	
