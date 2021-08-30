public class Statistics {
	public void averageFullBus() {
		for (int i = 0; i < BusSim.busses.length; i++) {
			System.out.println("The bus number " + i + " has " + BusSim.busses[i].getNumPass() + " Passengers");
		}
	}

	public void maxWaitTime() {
		Rider maxWaitRider = BusSim.busses[0].busArr[0]; // Make the firsr passenger the max wait time
		
		for (int i = 0; i < BusSim.busses.length; i++) {
			for (int j = 0; j < BusSim.busses[i].busArr.length; j++) {
				if(BusSim.busses[i] != null) {
					//if() check the wait time of the first passenger to the next and then keep reseting max, couldnt figure out how to get access to wait time
					
				}
					
			}
		}
		System.out.println("The average wait time for a passenger is " + Rider.getAvrWait() );
	}

	public void maxQueueLenth() {

	}
}
