/**
 * Priority Queue and Simulation
 *
 * A Priority Queue is used for an agenda
 *
 * This priority queue takes in IEvents with a certain priority (amount of time).
 *  Being a priority queue, the property is 'highest priority out first', highest
 *  priority in this case being the lowest time. The amount of time we pass in
 *  is added to the 'current time', and given that value in the queue. So if 
 *  we are at time 15, and pass in an IEvent with value time=5, that new IEvent will
 *  be given a time in the PQ of (15+5=)20, and will be processed once all IEvents 
 *  at times 15, 16, 17, 18, and 19 are processed.
 *
 * New items cannot be added at a time previous to current time; so you cannot 
 *  go back in time.
 */

public class PQ implements IPQ {

    // constructor

    public PQ() {
        seg = new Segment(0);
    }

    // methods

    public void add(IEvent o, double time) {

        time += getCurrentTime();
        
        if (time < seg.getTime())
          System.out.println("Error: trying to go back in time");
        else if (time == seg.getTime())
          seg.getEvents().add(o);
        else {  // search list for correct insertion point, then insert
          Segment trailer = seg, ptr = seg.getNext();
          while (ptr != null && time > ptr.getTime()) { //search
              ptr = ptr.getNext();
              trailer = trailer.getNext();
          }  // search
          if (ptr != null && time == ptr.getTime())
            ptr.getEvents().add(o);
          else {  // add new segment after trailer and before ptr
            Segment temp = new Segment(time);
            temp.getEvents().add(o);
            temp.setNext(ptr);
            trailer.setNext(temp); 
          }  // add new segment after trailer and before ptr
        }  // search list for correct insertion point, then insert
    }  // add method

    public IEvent remove() {
        if (this.isEmpty()) {
          System.out.println("Error: removing from empty queue");
          return null;
        }
        else if (seg.getEvents().length() == 0) {
          seg = seg.getNext();        
          return (IEvent) seg.getEvents().remove();
        }
        else return (IEvent) seg.getEvents().remove();
    }

    public boolean isEmpty() {
        return (seg.getEvents().length() == 0 && seg.getNext() == null);
    }

    public double getCurrentTime() { 
        return seg.getTime();
    }

    // instance variables

    private Segment seg;  // front of list representing priority queue

}  // PQ class 
