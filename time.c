volatile int FramesPerSecond=1;
volatile int Frames=0;
volatile int RefreshDecors=0;
void MainTimer(void) {
	FramesPerSecond=Frames;
	Frames=0;
	RefreshDecors=1;
}

volatile int FrameSignal=1;
void FrameTimer(void) {
	FrameSignal=1;
}