import java.lang.reflect.Array;
import java.util.Arrays;

public class Bus {

    public static Stop[] stops = new Stop[30];//added
    public Rider[] busArr; // Made public
    private int numPass;
    public static int busNum = 0;
    private int thisBusNum;

    private boolean isExpress;

    public Bus (boolean isExpress) {
        busArr = new Rider[50];
        numPass = 0;
        this.isExpress = isExpress;
        thisBusNum = busNum;
        busNum++;
    }

    public boolean getIsExpress() {return isExpress; }

    public int getNumPass() {return  numPass;}

    public int getBusNum() {return thisBusNum;}

    public boolean addPassenger(Rider passenger) {
        if (this.isFull())
            return false;
        busArr[numPass] = passenger;
        numPass++;
        return true;
    }

//    public boolean getIsExpress(){ return isExpress;}

    public Rider[] removeRider(Stop stop) {
        int numPassRemoved = 0;
        Rider[] passAtStop = new Rider[50];
        for (int i = 0; i < numPass; i++){
            if (busArr[i].getDepStop().equals(stop)){
                //busArr[i].setWaitTime(BusSim.agenda.getCurrentTime());
                //Rider.totWaitTime += busArr[i].getWaitTime();
                passAtStop[numPassRemoved] = busArr[i];
                numPassRemoved++;

            }
        }
        Rider[] passAtStopN = new Rider[numPassRemoved];
        passAtStopN = Arrays.copyOf(passAtStop, numPassRemoved);
        numPass -= numPassRemoved;
        compactBus();
        return  passAtStopN;
    }

    public boolean isFull() { return numPass >= busArr.length; }

    public void compactBus() {
        for (int i = 0; i < numPass; i++){
            if (busArr[i] == null){
                for(int j = i; i < numPass; j++)
                    busArr[j] = busArr[j + 1];
            }
        }
    }

    public boolean isEmpty() {return busArr[0] == null; }

    public String toString() {
        return Arrays.toString(busArr);
    }


}
