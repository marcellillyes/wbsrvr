from scipy import signal
import matplotlib.pyplot as plt
import numpy as np

points=4000
sampleintervalinms=2
chunksize=500
Fs=500
f1=45
f2=55

fn1=2*f1
fn2=2*f2

#IIR Direct Form 1 class

class IIR_filter ():
    def __init__(self,sos):
        self.b0=sos[0]
        self.b1=sos[1]
        self.b2=sos[2]
        self.a0=sos[3]
        self.a1=sos[4]
        self.a2=sos[5]
        self.mx1=0
        self.mx2=0
        self.my1=0
        self.my2=0
    
    def filterr (self,x):
        out= x*self.b0 + self.mx1*self.b1 + self.mx2*self.b2 - self.my1*self.a1 - self.my2*self.a2
        
        self.mx2=self.mx1
        self.my2=self.my1
        self.mx1=x
        self.my1=out
        
        return (out)


f1=open("5HZ.txt", 'r')

t=[]
y=[]

# Reading in data from text file
# Timestamp , Data 

for i in range(points):
    buf=""
    char=""
    while(char != "\n" and char != ","):
          
        buf+=char
        char=f1.read(1)
        
    if(char==","):
        t.append(int(buf))
    else:
        y.append(int(buf))
        
f1.close()

# Normalizing time for graphs

t = [x - t[0] for x in t]

# Calculating intervals

intervals=[]
for i in range(len(t)-1):
    intervals.append(t[i+1]-t[i])

# Performing FFT

f = np.real(np.fft.fft(y))
f[0]=0
fx=np.linspace(0,250,500)

# Creating filter coefficients

sos = signal.cheby2(N=2,rs=20, Wn=[fn1/Fs,fn2/Fs], btype='bandstop', output='sos')
b,a = signal.cheby2(N=2,rs=20, Wn=[fn1/Fs,fn2/Fs], btype='bandstop')
w,h =signal.freqz(b,a)

# Instanciating biquad filters

b1 = IIR_filter(sos[0])
b2 = IIR_filter(sos[1])

a=np.zeros(len(y))
b=np.zeros(len(y))

# Performing filtering

for i in range(len(y)):
    a[i]=b1.filterr(y[i])
    b[i]=b2.filterr(a[i])
    

# Plotting filtered frequency spectrum

fb=np.real(np.fft.fft(b[800:1800]))
#fb[0]=0
plt.plot(fx,20*np.log10(np.abs(fb[0:500])),label='ff')



# Plotting time signal

fig = plt.figure(figsize=(8,5))
ax = fig.add_subplot(1, 1, 1)

#Major ticks every 20, minor ticks every 5
major_ticks = np.arange(0, 10001, 200)
minor_ticks = np.arange(0, 10001, 20)

ax.set_xticks(major_ticks)
ax.set_xticks(minor_ticks, minor=True)

ax.grid(which='both')

ax.grid(which='minor', alpha=0.2)
ax.grid(which='major', alpha=0.5)

plt.plot(t[1000:1500],y[1000:1500])
plt.ylabel("Amplitude in 16 bit representation")
plt.xlabel("Time [ms]")







