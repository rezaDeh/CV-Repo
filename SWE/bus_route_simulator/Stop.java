public class Stop {

    private Q<Rider> line;
    private int stopNum;
    private String stopName;
    private boolean isExpressStop;
    private boolean isDownTown;


    public Stop(int stopNum) {
        line = new Q<Rider>();
        this.stopNum = stopNum;
        this.stopName = this.getStopName(stopNum);
    }


    public Q<Rider> getLine() {return line;}

    public int getStopNum() {
        return this.stopNum;
    }

    public boolean getIsDownTown() {return isDownTown; }

    public boolean getIsExpress() {return isExpressStop; }


    private String getStopName(int stopNum) {
        if (stopNum == 0) {
            isDownTown = true;
            isExpressStop = true;
            return "Ramp B";
        } else if (stopNum == 1 || stopNum == 29) {
            isDownTown = true;
            isExpressStop = true;
            return "4th & Nicollet";
        } else if (stopNum == 2 || stopNum == 28) {
            if (stopNum == 28)
                isExpressStop = true;
            return "Anderson Hall";
        } else if (stopNum == 3 || stopNum == 27)
            return "Jones Hall";
        else if (stopNum == 4 || stopNum == 26) {
            if (stopNum == 4)
                isExpressStop = true;
            return "Kasota Circle";
        } else if (stopNum == 5 || stopNum == 25)
            return "Como & Eustis";
        else if (stopNum == 6 || stopNum == 24) {
            if (stopNum == 24)
                isExpressStop = true;
            return "Como & Cleveland";
        } else if (stopNum == 7 || stopNum == 23)
            return "Como & Snelling";
        else if (stopNum == 8 || stopNum == 22) {
            if (stopNum == 8)
                isExpressStop = true;
            return "Como & Hamline";
        } else if (stopNum == 9 || stopNum == 21)
            return "Maryland & Dale";
        else if (stopNum == 10 || stopNum == 20) {
            if (stopNum == 20)
                isExpressStop = true;
            return "Maryland & Rice";
        } else if (stopNum == 11 || stopNum == 19)
            return "Front & Lexington";
        else if (stopNum == 12 || stopNum == 18) {
            if (stopNum == 12)
                isExpressStop = true;
            return "Front & Dale";
        } else if (stopNum == 13 || stopNum == 17)
            return "Capitol";
        else if (stopNum == 14 || stopNum == 16) {
            isDownTown = true;
            isExpressStop = true;
            return "Cedar";
        } else if (stopNum == 15) {
            isDownTown = true;
            isExpressStop = true;
            return "Union Depot";
        } else
            return null;
    }

    public String toString() {
        return "stop: " + String.valueOf(this.stopNum) + ", " + this.stopName;
    }
}
