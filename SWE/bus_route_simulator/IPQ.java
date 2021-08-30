// Priority Queue and Simulation

public interface IPQ {

    // Priority Queue Interface

    /** 
     * Places an event object into priority queue with priority given
     * by time 
     */
    public void add(IEvent o, double time);

    /** 
     * Removes and returns the highest priority event in a priority
     * queue; returns null if the priority queue is empty 
     */
    public IEvent remove();

    /** 
     * Returns true if the priority queue is empty, false otherwise 
     */
    public boolean isEmpty();
     
    /** 
     * Returns the priority associated with the top priority event in the
     * priority queue 
     */
    public double getCurrentTime();

}  // IPQ
