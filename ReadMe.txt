

*****************************VOWEL RECOGNITION**************************


========================================================================
    Instructions to run the file
========================================================================

Open the [214101050_vowelRecognition.sln] file in Visual Studio 10+.

Run the VowelRecognition.cpp ( F5 ).

=========================================================================
    Input
==========================================================================

Input recordings for training folder: 214101050_training_recordings/

Input recordings for tresting folder: 214101050_test_recordings/

=========================================================================
Steps for vowel recognition
=========================================================================

Steps to generate reference file for 1 vowel

1) Take first vowel recording, DC Shift if present has to be removed from all the sample values for the zero line correctness

2) Normalization of the sample data is done based on the threshold values of amplitudes ( +5000/-5000 )

3) Select 5 frames of the steady part ( taken frames around the maximum short term energy)

3) Compute Ri's Ai's ad Ci's of these frames and apply raised sine window on Ci's

4) Repeat the above steps for 4 more recording of the same vowel  and take the average of these 10 recordings.
so finally we will get 5 rows of Ci values( 5 rows and 12 columns). Dump these values in a text file. This becomes the reference file.

5) Similarly referance file will be generated for remaining 4 vowels and these 5 text files will be used as reference file for those vowels.   

=========================================================================
Testing
=========================================================================

1)Take input files for testing (10 test files per vowel) and pass these test files in a loop so that we can check out of those 10 files, how many are recognized correctly.

2) For each test file take 5 frames from the stable region

3) calculate the Tokhura's distance from each of the reference file (5 files).

4) The one with minimum distance will be recognized as a vowel. Report all five distances.

============================================================================
Naming Convention
============================================================================
 
Recordings : RollNo_vowel_utteranceNo.txt

File name after applying dc shift and normalization : RollNo_vowel_utteranceNo_normalized.txt

Referance file : RollNo_vowel_referance.txt

============================================================================
Accuracy
===========================================================================

Overall accuracy is 94%  

47/50 vowels are recognised correctly 
{a-100%, e-100%, i-70%, o-100%, u-100%} 

However, accuracy depends on the input recording data provided as well
