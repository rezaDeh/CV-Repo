// Example 27
// Queue implementation using an Array
// Revised November 2017

public class Q<T> implements IQ<T> {

    private T[] q;
    private int size;  // number of items in the array
    private int front;  // first element
    private int rear;  // last element

    // constructors

    public Q() {
        q = (T[]) new Object[0];
    }

    public Q(int initLength) {

        if (initLength < 0)
          throw new IllegalArgumentException("capacity requested is negative");
        q = (T[]) new Object[initLength];
    }

    // selectors

    public void add(T o) {
    
        if (q.length == 0) {  // array non-existant, create it and insert first object 
          q = (T[]) new Object[1];
          size = 1;
          q[0] = o; 
          rear = 0;
          front = 0;
        }
        else if (size == 0)  { // adding to empty queue
          rear = 0;
          front = 0;
          size = 1;
          q[0] = o;
        }
        else  {  // general case: array exists and non-empty
          if (size == q.length) {  // allocate bigger array if needed
            T[] newq = (T[]) new Object[2 * q.length + 1];
            if (front <= rear)  // queue has not wrapped, 
                                // so make simple copy to new space
              System.arraycopy(q, front, newq, 0, size);
            else if (front > rear) {  // queue has wrapped,
                                      // so copy in two chunks
                   System.arraycopy(q, front, newq, 0, q.length - front);
                   System.arraycopy(q, 0, newq, q.length - front, rear + 1);
                   front = 0; 
                   rear = size - 1;
                 }
            q = newq;
          }  // allocate bigger array if needed

          rear = (rear + 1) % q.length;
          q[rear] = o;  
          size++;

      }  // general case: array exists and non-empty
    }  // add

    public T remove() {

        if (size == 0)
          return null;

        T answer = q[front];
        front = (front + 1) % q.length;
        size--;
        return answer;
    }

    public int length() {
        return size;
    }

}  // Q class 
