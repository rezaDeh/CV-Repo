import java.util.Random;

public class Rider {
    private Stop arrStop;
    private Stop depStop;
    private double arrTime;
    private double waitTime;
    static public int numRiders;
    static public double totWaitTime;

    public Stop getArrStop() {return this.arrStop;}

    public Stop getDepStop() {return this.depStop;}

    public double getWaitTime() { return waitTime; }

    public void setWaitTime(double travTime) { waitTime = travTime - arrTime; }

    public static double getAvrWait() { return totWaitTime / numRiders; }

    public String toString () {
        return "pickup: " + this.arrStop + " dropoff: " + this.depStop;
    }

    public Rider (Stop arrStop){
        this.numRiders ++;
        this.arrStop = arrStop;
        this.arrTime = BusSim.agenda.getCurrentTime();
        Random rand = new Random();
        this.depStop = BusSim.stops[rand.nextInt(30)];
        while(this.depStop.getStopNum() < this.arrStop.getStopNum()){
            //System.out.println("choosing... depStop, arrStop: (" + depStop.getStopNum() + ", " + this.arrStop.getStopNum() + ")");
            this.depStop = BusSim.stops[rand.nextInt(30)];
        }
    }
}
