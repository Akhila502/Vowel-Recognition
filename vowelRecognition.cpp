// vowelRecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;


long double Ci_frames[100][13];
char vowel[5] = {'a', 'e', 'i', 'o', 'u'};

//function prototypes
void calculate_normalization(const char *, string);
int frame_with_max_ste(const char *);
void calculating_coefficients_for_5_frames_around_max_ste(const char *, int, int &);
void calculate_Ri(long double *, vector<long double> &s);
void calculate_Ai(long double *, long double [13][13]);
void calculate_Ci(long double [13][13], long double *, int &);
long double raisedSin(int);
void create_referance_file(const char *);
void testing(string);
vector<vector<long double>> referance_file_data(string);
void tokhura_distance(vector<vector<long double>> &, vector<vector<long double>> &, long double &);
void test();


int _tmain(int argc, _TCHAR* argv[])
{
	//-----------------------------------------------------------------------------------\------
	//TRAINING
	//------------------------------------------------------------------------------------------
	for( int i = 0; i < 5; i++ ) //to loop through each vowel
	{
		for(int j = 0; j < 10; j++) //to loop through each recording of one vowel
	    {
			int frame_no = 0;
			char s[100];
			sprintf( s, "214101050_training_recordings/214101050_%c_%d",  vowel[i], j + 1 );
			string str = s;
			string s1 = s;
			str += ".txt";
			const char *orig_file= str.c_str();
			calculate_normalization(orig_file, s1);   //calculate normalizationa and dc shift for the each recording and store it into a new file
			char s2[100];
			sprintf( s2, "214101050_training_recordings/214101050_%c_%d_normalized.txt",  vowel[i], j + 1 );
			const char *normalized_file = s2;
			int frame = frame_with_max_ste( normalized_file );  //In the normalized file, retrieve the frame number having maximum short term energy
			calculating_coefficients_for_5_frames_around_max_ste( normalized_file, frame, frame_no ); // calculate Ai, Ri and Ci
		}
		char s[100];
		sprintf(s, "214101050_test_recordings/214101050_%c_referance.txt",  vowel[i]);
		const char *ref_file= s;
		create_referance_file(ref_file);  //create a referance file for each vowel 

	}

	//at this point, referance file for each vowel has been created which can be used for testing
	//--------------------------------------------------------------------------------------------
	//TESTING
	//--------------------------------------------------------------------------------------------
	for( int i = 0; i < 5; i++ )           //to loop through each vowel
	{
		for(int j = 0; j < 10; j++)        //to loop through each recording of one vowel
	    {
			int frame_no = 0;
			char s[100];
			sprintf(s, "214101050_test_recordings/214101050_%c_%d",  vowel[i], j + 11);
			string str = s;
			string s1 = s;
			str += ".txt";
			const char *orig_file = str.c_str();
			calculate_normalization(orig_file, s1);  //calculate normalizationa and dc shift for the each recording and store it into a new file
			char s2[100];
			sprintf(s2, "214101050_test_recordings/214101050_%c_%d_normalized.txt",  vowel[i], j + 11);
			const char *normalized_file = s2;
			int frame = frame_with_max_ste(normalized_file); //In the normalized file, retrieve the frame number having maximum short term energy
			calculating_coefficients_for_5_frames_around_max_ste(normalized_file, frame, frame_no); // calculate Ai, Ri and Ci

		    testing(normalized_file); //testing each recording 
		}
	}
	//test();
	return 0;
}


//calculate normalizationa and dc shift for the each recording and store it into a new file
void calculate_normalization(const char *orig_file, string str)
{
	FILE *fin;
	FILE *fout;
	long double normalization, dc_shift;
	long double count = 0, max_data = 0, min_data = 0;
	long double sum = 0;
	char singleLine[100];
	int flag=0;

	string s = str;
	s += ".txt";
	str += "_normalized.txt"; 
	const char *normalized_file= str.c_str();

	fin = fopen(orig_file, "r");   //open the original recording 
	while( !feof(fin) )
	{
		fgets(singleLine, 100, fin);  
		if( !isalpha(singleLine[0]) )  //skip the starting lines of recording which has buffer data
		{
			count++;
			long double data = atof(singleLine);
			sum += data;
			if( data > max_data )
				max_data = data;       //retrieve the maximum amplitude in the recording
			if( data < min_data )
				min_data = data;       //retrieve the minimum amplitude in the recording
			if( count >= ( 320 * 3 ) && flag == 0)   //consider first 3 frames for dc_shift calculation as it contains the silence
			{
				dc_shift = sum / 960;
				flag = 1;
				count = 0;
			}
		}
	}

	if( abs(min_data) > abs(max_data) )
		max_data = abs(min_data);
	normalization = ( max_data - 5000 ) / max_data;  //calculate the normalization factor
	fout= fopen(normalized_file, "w");               //save the normalized data into a new file

	rewind(fin);  //move the file pointer to beginning                        
	while( !feof(fin) )
	{
		fgets(singleLine, 100, fin);   
		if( !isalpha(singleLine[0]) )    //skip the starting lines of recording which has buffer data
		{
			long double data = atof(singleLine);
			long double norm_data = ( data - dc_shift ) * normalization;// calculate the normalized data and store into the new file
			fprintf(fout, "%lf\n", norm_data);
		}
	}

	//close the files opened
	fclose(fin);
	fclose(fout);
}

//In a recording, the frame having the maximum short term energy is found
int frame_with_max_ste(const char *fname)
{
	FILE *fin;
	int flag = 0;
	int frame_count = 0;
	long double sum = 0, max_ste = 0;
	long int count = 0, frame;
	char singleLine[100];

	fin = fopen(fname, "r");
	while( !feof(fin) )
	{
		fgets(singleLine, 100, fin);  
		long double data = atof(singleLine);
		count++;   //to keep track of single frame i.e., 320 samples of data
		sum += ( data * data );
		if(count==320) //for evrey frame, calculate ste
		{
			frame_count++;  //keep track of frame number
			sum /= 320;  
			if(sum > max_ste)
			{
				max_ste = sum;
				frame = frame_count;
			}
			count = 0;
			sum = 0.0;
		}
	}
	fclose(fin);
	return frame;  //return the frame number with maximum ste
}


//calculating the Ai, Ri and Ci values for the 5 frames around maximum ste and keeping track of Ci values in Ci_frames
void calculating_coefficients_for_5_frames_around_max_ste(const char *fname,int frame, int &frame_no)
{
	FILE *fin;
	long double R[13] = {0}, A[13][13], C[13] = {0};    //for storing the Ri, Ai and Ci values for each frame
	int count=0, count_frame=0;
	vector<long double> signal;                         //for storing the data for a single frame
	vector<long double> frame_data;                     // for storing the data for 5 frames of a recording 
	char singleLine[100];
	long double data;

	fin = fopen(fname, "r");
	while( !feof(fin) )
	{
			fgets(singleLine, 100, fin); 
			data = atof(singleLine);
			count++;
			if(count == 320)    //to keep track of number of frames 
			{
				count_frame++;
				count = 0;
			}
			if(count_frame == ( frame - 3 ))   //when frame around the maximum ste reached, store amplitudes of 5 frames around maximum ste into vector frame_data
			{
				int f_count=0;
				while(f_count <= (5 * 320) - 1) //store amplitudes of 5 frames around maximum ste into vector frame_data
				{
					frame_data.push_back(data);
					f_count++;
					fgets(singleLine, 100, fin); 
					data = atof(singleLine);
				}
				int i = 1;
				f_count=0;
				while(f_count < frame_data.size())
				{
					//out of those 5 frames, we are storing each frame in signal and calculating ri,ci and ai.
					while(f_count <= (320 * i) - 1)
					{
						signal.push_back(frame_data[f_count]);
						f_count++;
					}
					calculate_Ri(R, signal);
					calculate_Ai(R, A);
					calculate_Ci(A, C, frame_no);
					frame_no++;
					signal.clear();
					i++;
				}
				frame_data.clear();
			}
	}
	fclose(fin);
}

//To calculate Ri values for a single frame
void calculate_Ri(long double *R, vector<long double> &signal)
{

	for(int i = 0; i <= 12; i++)
	{
		R[i] = 0;
		for(int j = 0; j < 320 - i; j++)
			R[i] += signal[j] * signal[j+i];
	}
}

//To calculate Ai values for a single frame
void calculate_Ai(long double *R, long double A[13][13])
{
	long double E[13] = {0}, k[13] = {0};

	E[0] = R[0];

    E[1] = ( ( R[0] * R[0] ) - ( R[1] * R[1] ) ) / R[0 ];
    k[1] = R[1] / R[0];
    A[1][1] =  k[1];

    for(int i = 2; i < 13; i++)
    {
         long double sum=0;
         for(int j = 1; j <= i - 1; j++)
		 {
			sum  += ( A[j][i-1] * R[i-j] );
		 }
         k[i] = (R[i] - sum) / E[i-1];
         A[i][i] = k[i];
         E[i] = ( 1 - ( k[i] * k[i] ) ) * E[i-1];  
         for(int j = 1; j <= i-1; j++)
		 {
             A[j][i] = A[j][i-1] - ( k[i] * A[i-j][i-1] );
		 }
     }
			
}

//To calculate the Ci values for a single frame and keep track of Ci values for a vowel into Ci_frames
void calculate_Ci(long double A[13][13], long double *C, int &frame_no)
{
	//calculate the Ci values for one frame
	for(int i = 1; i <= 12; i++)
    {
		long double sum = 0;
		for(int j = 1; j <= i - 1; j++)
			sum += ( ( j / (i * 1.0)) * C[j] * A[i-j][12] );
		C[i] = sum + A[i][12];
	}
	//storing the Ci values related to recordings of one vowel into Ci_frames
	Ci_frames[frame_no][0] = 0;
	for(int i = 1; i <= 12; i++)
	{
		Ci_frames[frame_no][i] = C[i] * raisedSin(i);
	}
}

//for applying the raised sin window on Ci's
long double raisedSin(int m)
{
	return 1 + ( 12 * 1.0 / 2 ) * sin( 3.14 * m / 12 );
}

//To create the referance file for a single vowel
void create_referance_file(const char *fname )
{
	FILE *fout;
	int count = 0; 
	long double avg;

	fout = fopen(fname, "w");   //create and open referance file for a vowel
	while( count != 5 )
	{
		for(int i = 1; i <= 12; i++)
		{
			long double sum = Ci_frames[count][i];
			for(int j = (count + 5); j < 50; j += 5)
			{
				sum += Ci_frames[j][i];
			}
			avg = sum / (10 * 1.0);    //calculate the average of 10 recordings of a vowel
			fprintf(fout, "%lf\n", avg);
		}
		count++;
	}
	fclose(fout);
}

//to test each recording against the referance files generated
void testing(string fname)
{
	vector<vector<long double>> Ci_referance;
	vector<vector<long double>> Ci_test;
	char predict;
	long double min=9999999.000,final_dist=0.0;
	
	//retrieve the Ci values of test data of one recording into Ci_test
	for(int i = 0; i < 50; i += 5)
	{
		for(int j = 0; j < 5; j++)
		{
			Ci_test.push_back(vector<long double>());
			for(int k = 1; k <= 12; k++)
			{	
				Ci_test[j].push_back(Ci_frames[i+j][k]);
			}
		}
	} 

	for(int i = 0; i < 5; i++)
	{
		string s;
		s="214101050_test_recordings/214101050_";
		s += vowel[i];
		s+="_referance.txt";
		//get the referance file into the vector Ci_referance
		Ci_referance = referance_file_data(s);
		//calculate tokhura distance for the test vs each referance data
		tokhura_distance(Ci_referance, Ci_test, final_dist);
		Ci_referance.clear();
		//keep track of the minimum tokhura distance
		if(final_dist < min)
		{
			min = final_dist;
			predict = vowel[i];
		}
		final_dist=0.0;
	}
	printf("Vowel Predicted :: %c \n\n", predict);
	Ci_test.clear();
}

//retrieving referance file into vector for calculating tokhura distance
vector<vector<long double>> referance_file_data(string filename)
{
	vector<vector<long double>> ci_ref;
	long double data;
	ifstream fin;
	fin.open(filename);
	for(int i = 0; i < 5; i++)
	{
		ci_ref.push_back(vector<long double>());
		for(int j = 0; j < 12; j++)
		{
			fin >> data;
			ci_ref[i].push_back(data);
		}
	}
	return ci_ref;
}

//calculates tokhura distance for each recording taken as test file and referance file
void tokhura_distance(vector<vector<long double>> &ci_ref, vector<vector<long double>> &ci_test, long double &final_dist)
{
	long double tokuhara_dist, dif;
	long double weight[]={ 1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0 };
	for(long unsigned int i = 0; i < ci_ref.size(); i++)
	{
		tokuhara_dist = 0;
		for(int j = 0; j < 12; j++)
		{
			dif  = (ci_ref[i][j] - ci_test[i][j]) ;
			tokuhara_dist += (dif * dif * weight[j]);
		}
		final_dist += tokuhara_dist;
	}
}

//This function has been created to verify whether the values of Ri's , Ai's and Ci's are correct or not
//Verifies the values and not using now as the values came out to be correct
void test()
{
	ifstream fin;
	vector<long double> signal;
	int x=1;
	long double R[13]={0}, A[13][13], C[13]={0};
	fin.open("test.txt");
	double w;
	int count = 0;
	while(fin >> w)
	{
        signal.push_back(w);
		printf("%lf\n", w);
	}
	calculate_Ri(R, signal);
	for(int i=0;i<13;i++)
		printf("R[%d] = %lf\n", i, R[i]);
	calculate_Ai(R, A);
	for(int i=0;i<13;i++)
		printf("A[%d] = %lf\n", i, A[i][12]);
	calculate_Ci(A, C, x );
	for(int i=0;i<13;i++)
		printf("C[%d] = %lf\n", i, Ci_frames[1][i]);
	printf("___________----------------------------------__________\n");

}
