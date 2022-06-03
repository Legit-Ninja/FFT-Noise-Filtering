
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include "pbPlots.h"
#include "supportLib.h"
#include <C:/Users/bgreenwood/AppData/Roaming/FFT/fftw3.h>
//help from example: http://www.fftw.org/fftw3.pdf

//this data_size is directly related to what range of frequencies are measured- freq range: (0-data_size/2)
#define data_size 1000
//process oulined in python by Prof. Steve Brunton https://www.youtube.com/watch?v=s2K1JfNR7Sc&ab_channel=SteveBrunton
int main()
{ 
    //array for x and y coordinates
    double xs[data_size];
    double ys[data_size];
    //this value has to be equal to 1/data_size
    double dt = .001;

    //ask user for freq and phase of sine wave 
    double freq;
    printf("enter freq: ");
    scanf("%lf", &freq);
    fflush(stdin);
    printf("you entered: %lf,\n", freq);
    double time_t = 0.0;
    //plot out data_size points of the wave, in .05sec intervals
    for (int i =0; i < data_size; i++)
    {
        //regular sin wave w/ freq , in increments of 0.005
        xs[i] = time_t;
        //printf("freq, pi, cur x-> %lf, %lf, %lf \n",freq, M_PI, time); //debugging
        ys[i] = sin(time_t*freq*2.0*M_PI);
        // increase the time quantum
        time_t += dt;
    }
    time_t = 0.0;
    //adding the 25 hz and random noise
    srand(time(0));
    double xs1[data_size];
    double ys1[data_size];
    float rand_amount;
    for (int i =0; i < data_size; i++)
    {
        rand_amount = (float)rand()/((RAND_MAX/2)-1);
        //regular sin wave w/ freq , in increments of 0.005
        xs1[i] = time_t;
        //printf("freq, pi, cur x-> %lf, %lf, %lf \n",freq, M_PI, time); //debugging
        ys1[i] = sin(time_t*25.0*2.0*M_PI);
        // increase the time quantum
        time_t += dt;
        xs[i] += xs1[i];
        ys[i] += ys1[i] + rand_amount*2.0;
    }

    //tells us whether the draw worked
    _Bool success;

    //need to have this
	RGBABitmapImageReference *canvasReference = CreateRGBABitmapImageReference();
    //custom error message from pbPlot implementation
	StringReference *errorMessage = (StringReference *)malloc(sizeof(StringReference));
    //draw the plot (reference, width, height, x vals, # of x vals, y vals, # of y vals, errorstring)
	success = DrawScatterPlot(canvasReference, 600, 400, xs, 50, ys, 50, errorMessage);
    //if the draw was successful
    if(success){
        size_t length;
        //turn the generated graph into a png 
        double *pngdata = ConvertToPNG(&length, canvasReference->image);
        WriteToFile(pngdata, length, "graph_with_noise.png");
        //clean up after memory
        DeleteImage(canvasReference->image);
	}
    else{ //it was not successful
        //say why it didn't work
	    fprintf(stderr, "Error: ");
	    for(int i = 0; i < errorMessage->stringLength; i++){
            fprintf(stderr, "%c", errorMessage->string[i]);
        }
        fprintf(stderr, "\n");
    }

    ////////////////////////////////////////////////////////////
    //now we FFS that data  
    ////////////////////////////////////////////////////////////
    int N = data_size;
    //complex: double[2] = {real_part,imag_part}
    fftw_complex *in, *out;
    fftw_plan p;
    //initialize the arrays-> "in" is an array of fftw_complex type (basically a pair of doubles)
    //in is f (set of points we know) -> out is fhat (complex fourier coefficents) with magnitude and phase
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    printf("in/out arrays initailized...  \n");
    printf("n is: %d \n", N);
    // makes the "plan" to solve the DFT (n objects, input array, output array, regular/inverse, quickAnswer(estimate)/bestWay(meaure))
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_MEASURE);
    printf("DFT sucessfully planned... \n");
    //pass in the data to be transformed
    for (int i =0; i < data_size; i++)
    {
        in[i][0] = ys[i];
    }
    //do the fft
    fftw_execute(p);

    //get the magnitude of the "out" results 
    double PSD[data_size]; //power density, y-axis
    double f[data_size]; //frequency, x-axis
    int L[data_size];
    for (int i =0; i < data_size; i++)
    {           //a^2 + b^2
        PSD[i] = (out[i][0] * out[1][0] + out[i][1] *out[i][1]);
        f[i] = (1/(dt*N) * i);
    	//printf("i: %d, PSD(Y): %lf, freq(x): %lf \t", i, PSD[i], f[i]); debugging
    }
    //determine the range of frequencies
    for (int i =0; i < (sizeof(L)/sizeof(double)); i++)
    {
        L[i] = i;
    }
    printf("drawing the resulting graph...\n");
    //scale the PSD and freqencies 
    double PSD_L[data_size];
    double f_L[data_size];
    for (int i =0; i < data_size; i++)
    {           //a^2 + b^2
        PSD_L[i] = PSD[L[i]];
        f_L[i] = f[L[i]];
    }
    // draw the resulting psd vs freq graph
    //customize the graph
    ScatterPlotSeries *series = GetDefaultScatterPlotSeriesSettings();
	series->xs = f_L;
	series->xsLength = data_size/2;
	series->ys = PSD_L;
	series->ysLength = data_size/2;
	series->linearInterpolation = true;
	series->lineType = L"solid";
	series->lineTypeLength = wcslen(series->lineType);
	series->lineThickness = 2;
	series->color = GetGray(0.3);

	ScatterPlotSettings *settings = GetDefaultScatterPlotSettings();
	settings->width = 600;
	settings->height = 400;
	settings->autoBoundaries = false;
    settings->xMax = data_size/2;
    settings->xMin = 0;
    settings->yMax = 50000;
    settings->yMin = 0; 
	settings->autoPadding = true;
    settings->title = L"FFT'd graph";
    settings->titleLength = wcslen(settings->title);
    settings->xLabel = L"Frequency(hz)";
    settings->xLabelLength = wcslen(settings->xLabel);
    settings->yLabel = L"PSD(power)";
    settings->yLabelLength = wcslen(settings->yLabel);
	ScatterPlotSeries *s [] = {series};
	settings->scatterPlotSeries = s;
	settings->scatterPlotSeriesLength = 1;
    //need to have this
	RGBABitmapImageReference *canvasReference2 = CreateRGBABitmapImageReference();
    //custom error message from pbPlot implementation
	StringReference *errorMessage2 = (StringReference *)malloc(sizeof(StringReference));
    //draw the plot (reference, width, height, x vals, # of x vals, y vals, # of y vals, errorstring)
	success = DrawScatterPlotFromSettings(canvasReference2, settings, errorMessage2);
    //if the draw was successful
    if(success){
        size_t length;
        //turn the generated graph into a png 
        double *pngdata = ConvertToPNG(&length, canvasReference2->image);
        WriteToFile(pngdata, length, "fft'd_graph.png");
        //clean up after memory
        DeleteImage(canvasReference2->image);
	}
    else{ //it was not successful
        //say why it didn't work
	    fprintf(stderr, "Error: ");
	    for(int i = 0; i < errorMessage2->stringLength; i++){
            fprintf(stderr, "%c", errorMessage2->string[i]);
        }
        fprintf(stderr, "\n");
    }

    printf("DONE!");
    return 0;
}
