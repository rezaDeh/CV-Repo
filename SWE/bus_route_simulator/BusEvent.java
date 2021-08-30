public class BusEvent implements IEvent {

	private Stop curStop;
	private Bus bus;

//	private int[] stopSelect = { 0, 0, 1, 1, 29, 29, 14, 14, 15, 15, 16, 16, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 17,
//			18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28 };

	public BusEvent(Stop curStop, Bus bus) {
		this.curStop = curStop;
		this.bus = bus;
	}

	public void run() {

		double arrivalT = 0;
		double departT = 0;
		double totalT = 0;

		if (!this.bus.isEmpty()) {
			Rider[] arrived = this.bus.removeRider(this.curStop);
			double curTime = BusSim.agenda.getCurrentTime();
			arrivalT = 2 * arrived.length;
		}

		while (!bus.isFull() && BusSim.stops[curStop.getStopNum()].getLine().length() > 0) {
			bus.addPassenger(BusSim.stops[curStop.getStopNum()].getLine().remove());
			departT += 3;
		}

		totalT = departT + arrivalT;

		// 15 second rule implementation not sure if it works
		if (!bus.isFull() && totalT < 15 && BusSim.stops[curStop.getStopNum()].getLine().length() <= 0) {
			for (int i = 0; i < 15 - totalT; i++) {
				if (curStop.getLine().length() > 0) {
					while (!bus.isFull() && BusSim.stops[curStop.getStopNum()].getLine().length() > 0) {
						bus.addPassenger(BusSim.stops[curStop.getStopNum()].getLine().remove());
						totalT += 3;
					}
				}
				if (totalT > 15)
					break;
			}

		}

		else if ((bus.isFull() || totalT >= 15) || BusSim.stops[curStop.getStopNum()].getLine().length() <= 0) {
			if (bus.getIsExpress()) {
				if (curStop.getStopNum() == 0 || curStop.getStopNum() == 14 || curStop.getStopNum() == 15
						|| curStop.getStopNum() == 15) {
					BusEvent bEN = new BusEvent(BusSim.stops[curStop.getStopNum() + 1], bus);
					BusSim.agenda.add(bEN, 240 + totalT);
				} else if (curStop.getStopNum() == 29) {
					BusEvent bEN = new BusEvent(BusSim.stops[0], bus);
					BusSim.agenda.add(bEN, 240 + totalT);
				} else if (curStop.getStopNum() == 12) {
					BusEvent bEN = new BusEvent(BusSim.stops[14], bus);
					BusSim.agenda.add(bEN, 480 + totalT);
				} else {
					BusEvent bEN = new BusEvent(BusSim.stops[curStop.getStopNum() + 4], bus);
					BusSim.agenda.add(bEN, 960 + totalT);
				}
			} else {
				if (curStop.getStopNum() == 29) {
					BusEvent bEN = new BusEvent(BusSim.stops[0], bus);
					BusSim.agenda.add(bEN, 240 + totalT);
				} else {
					BusEvent bEN = new BusEvent(BusSim.stops[curStop.getStopNum() + 1], bus);
					BusSim.agenda.add(bEN, 240 + totalT);

				}
			}

		}
		System.out.println("Bus Event " + curStop + ", Number of bus Passengers on bus # " + bus.getBusNum() + " is : "
				+ bus.getNumPass() + ", Current Time is: " + BusSim.agenda.getCurrentTime());
	}

}
