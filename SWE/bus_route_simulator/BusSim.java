import java.util.Random;

public class BusSim {
	public static int numBusses = 8 ;//added
	public static PQ agenda = new PQ();
	public static Stop[] stops = new Stop[30];//added
	public static Bus[] busses = new Bus[numBusses]; //added
	static int load = 120;
//	public static int heavyLoad = 120;
//	public static int lightLoad = 25;

	public static void main(String[] args) {

		//Added

        agenda.add(new RiderEvent(), 0);

		for (int i = 0; i < stops.length; i++) {
			Stop NS = new Stop(i);
			stops[i] = NS;
		}


        for (int i = 0; i < numBusses; i++) {
			Random random = new Random();
			//int randomNumber = random.nextInt(100);
			Bus newBus;
			if (i % 4 == 0) {
				newBus = new Bus(true); //Express
			}
			else {
				newBus = new Bus(false);

			}
			busses[i] = newBus;
			//int randomStop = random.nextInt();
			BusEvent newBusEvent = new BusEvent(stops[i * 4], newBus);
			agenda.add(newBusEvent, 110);
		}

		while (agenda.getCurrentTime() <= 2000) {
			agenda.remove().run();
		}

		for (int i = 0; i < 30; i++) {
			System.out.println(stops[i] + " is " + stops[i].getLine().length() + " long");
		}

		Statistics s = new Statistics();
		s.averageFullBus();
		s.maxQueueLenth();
		s.maxWaitTime();

	}
}
