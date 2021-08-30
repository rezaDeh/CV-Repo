import java.util.Random;

public class RiderEvent implements IEvent {

    private double[] arrivalPercents = { .75, .75, .5, .5, .5, .2, .2, .2, .2, 0, 0, -.2, -.2, -.2, -.2, -.5, -.5, -.5,
            -.75, -.75 };
    private int[] stopSelect = { 0, 0, 1, 1, 29, 29, 14, 14, 15, 15, 16, 16, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 17,
            18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28 };


    public double getArrivalTimeN() {
        Random rand = new Random();
        int randomTime = rand.nextInt(20);
        double time = 36 / (BusSim.load + arrivalPercents[randomTime] * BusSim.load);
        return time;
    }

    public double getArrivalTimeDT() {
        Random rand = new Random();
        int randomTime = rand.nextInt(20);
        double time = 36 / ((BusSim.load / 2) + arrivalPercents[randomTime] * (BusSim.load / 2));
        return time;
    }


    public int getArrivalStop() {
        Random random = new Random();
        int randomStop = random.nextInt(36);
        return stopSelect[randomStop];
    }

    public void run() {

        //System.out.println("running riderevent");
        int stopNum = getArrivalStop();
        Rider passenger = new Rider(BusSim.stops[stopNum]);
        RiderEvent rE = new RiderEvent();

        String outPut = "Rider Event " + BusSim.stops[stopNum] +
                ", Current Time is: " + BusSim.agenda.getCurrentTime() + ", Next Rider in: ";

        if(BusSim.stops[stopNum].getIsDownTown()) {
            double arriveDT = getArrivalTimeDT();
            BusSim.agenda.add(rE, arriveDT);
            System.out.println(outPut + arriveDT);
        }
        else {
            double arriveNorm = getArrivalTimeN();
            BusSim.agenda.add(rE, arriveNorm);
            System.out.println(outPut + arriveNorm);
        }

        BusSim.stops[stopNum].getLine().add(passenger);

    }


}
