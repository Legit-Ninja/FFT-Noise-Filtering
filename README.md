Author: Ben Greenwood, Infinetix Corp, Jun 2022

This code was written to determine what frequencies are found in a given digital signal.
I used the FFTW3 Library to compute the Fast-Fourier Transform, as well as pbPlot for graphical representations.
http://www.fftw.org/index.html
https://github.com/InductiveComputerScience/pbPlots

How To Use:
1) download this repo, as well as FFTW3 http://www.fftw.org/download.html
2) Change the path in the "include" statement in fft_noise_filter.c to wherever you saved the FFTW3 files, ex '#include "c:\users\...\fftw3.h" '
3) I used gcc to build, you can use whatever C compiler you like just note the build commands may differ
4) in your terminal, navigate to where the repository was copied. type 'gcc plot.c pbPlots.c supportLib.c -lfftw3-3 -Lc:\users\...\path_To_Where_FFTW3_Files_Are
5) Run 'a.exe'
6) It is set up now witha  simulated frequency, where the program combines the signal waves of a pure 25hz signal with another pure signal with user specifed frequency, and adds some random noise on top of that
7) the resulting '.png' files show the generated simulated signal, as well as the result of the FFT displaying PowerSignalDensity vs Frequencies
If all went right, you should see nice big spike at both 25hz and the other entered frequency

Note: This only handles frequencies between 0 and 100 hz. If you want more, you will have to adjust the code to account for that  