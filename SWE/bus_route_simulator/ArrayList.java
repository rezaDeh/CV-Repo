public class ArrayList<T extends Comparable<T>> {
	private T[] arrayList;

	// Constructor
	public ArrayList() {
		arrayList = (T[]) new Comparable[50];
	}


	// Adds element to the end of the List
	public boolean add(T element) {
		if (element == null && size() == arrayList.length)
			return false;
		arrayList[size()] = element;
		return true;

	}


	// Adds element at given index
	public boolean add(int index, T element) {
		if (element == null || index < 0 || index >= arrayList.length)
			return false;
		else {
			if (size() == arrayList.length) {
				growList();
			}
			for (int i = size(); i > index; i--) {
				arrayList[i] = arrayList[i - 1];
			}
			arrayList[index] = element;
			return true;
		}
	}


	// Clears out list
	public void clear() {
		for (int i = 0; i < arrayList.length; i++)
			arrayList[i] = null;
	}


	// Returns true if list contains element
	public boolean contains(T element) {
		boolean exists = false;
		for (int i = 0; i < arrayList.length; i++) {
			if (arrayList[i] == element)
				exists = true;
		}
		return exists;
	}


	// Returns the element at given index.
	public T get(int index) {
		if (index < 0 || index >= arrayList.length)
			return null;
		else
			return arrayList[index];
	}


	// Returns the first index of element in list
	public int indexOf(T element) {
		if (element == null)
			return -1;
		for (int i = 0; i < arrayList.length; i++) {
			if (arrayList[i] != null && arrayList[i].equals(element))
				return i;
		}
		return -1;
	}


	// Returns the last index of element in list
	public int lastIndexOf(T element) {
		if (element == null)
			return -1;
		for (int i = arrayList.length - 1; i >= 0; i--) {
			if (arrayList[i] != null && arrayList[i].equals(element))
				return i;
		}
		return -1;
	}


	// Returns true if list is empty
	public boolean isEmpty() {
		int numEmptyIndex = 0;
		for (int i = 0; i < arrayList.length; i++) {
			if (arrayList[i] == null)
				numEmptyIndex++;
		}
		return numEmptyIndex == arrayList.length;
	}


	// Replace the element at index with element and return the
	// Element that was previously at index.
	public T set(int index, T element) {
		if (element == null || index < 0 || index >= arrayList.length)
			return null;
		else {
			T replacedElement = arrayList[index];
			arrayList[index] = element;
			return replacedElement;
		}
	}


	// Return the number of elements in the list.
	public int size() {
		int listSize = 0;
		for (int i = 0; i < arrayList.length; i++) {
			if (arrayList[i] != null)
				listSize++;
		}
		return listSize;
	}


	/*
	 * Sort the elements of the list. If order is true, sort the list in increasing
	 * (alphabeticaly) order. If order is false, sort the list in decreasing
	 * (reverse alphabetical) order
	 */
	public void sort(boolean order) {
		for (int i = 0; i < size(); i++) {
			int m = i;
			for (int j = i + 1; j < size(); j++) {
				if (order) {
					if (arrayList[m].compareTo(arrayList[j]) > 0) {
						m = j;
					}
				} else {
					if (arrayList[m].compareTo(arrayList[j]) < 0) {
						m = j;
					}
				}
			}
			T tmp = arrayList[m];
			arrayList[m] = arrayList[i];
			arrayList[i] = tmp;
		}

	}

	
	// Remove the first instance of element from the list
	public boolean remove(T element) {
		boolean returnValue = false;
		for (int i = 0; i < (arrayList.length - 1); i++) {
			if (element == arrayList[i] || element.equals(arrayList[i])) {
				arrayList[i] = null;
				returnValue = true;
				for (int j = i; j < arrayList.length - 1; j++) {
					arrayList[j] = arrayList[j + 1];
				}
				break;
			}
		}
		arrayList[arrayList.length - 1] = null;
		return returnValue;
	}


	// Remove whatever is at index index in the list and return it
	public T remove(int index) {
		T removedElement = null;
		if (index >= 0 && index < size()) {
			removedElement = arrayList[index];
			// arrayList[index] = null;
			for (int i = index; i < size(); i++) {
				arrayList[i] = arrayList[i + 1];
			}
		}
		return removedElement;
	}

	

	// For n elements in list, if list is full, make a new list with length 2n
	public void growList() {
		T[] tempArray = (T[]) new Comparable[arrayList.length * 2];
		System.arraycopy(arrayList, 0, tempArray, 0, arrayList.length);
		arrayList = tempArray;
	}


	public String toString() {
		String arrayListStr = "";
		for (int i = 0; i < size(); i++) {
			arrayListStr += String.valueOf(i + 1) + "." + arrayList[i] + "\n";
		}
		return arrayListStr += " ";
	}


	public static void main(String[] args) {
        ArrayList a = new ArrayList();
        a.add('c');
        System.out.println(a);
        a.add('a');
        System.out.println(a);
        a.add('b');
        System.out.println(a);
        a.sort(false);
        System.out.println(a);
        System.out.println(a.indexOf('a'));
        a.remove(1);
        System.out.println(a);
		 a = new ArrayList();
		a.add("a");
		a.add("d");
		a.add("b");
		a.add("h");
		a.add("b");
		a.add("f");
		a.add("c");
		a.remove("b");
		System.out.println(a);
		a.remove(1);
		System.out.println(a);
		a.add(1, "g");
		System.out.println(a);
		a.sort(false);
		System.out.println("Sorted");
		System.out.println(a);
		a.remove("h");
		System.out.println(a);
		a.remove(4);
		System.out.println(a);
		a = new ArrayList<String>();
		System.out.println(a.isEmpty()); // Right Answer: True
		a.add("a");
		a.add("b");
		System.out.println(a.add("zzzzzzz"));// True
		System.out.println(a.add(null)); // False
		System.out.println(a.size()); // Right Answer: 3
		System.out.println(a.contains("Joe")); // Right Answer: False
		System.out.println(a.contains("b")); // Right Answer: True
		System.out.println(a.get(2)); // zzzzzzzzz
		System.out.println(a.get(80)); // null
		System.out.println(a.indexOf("b")); // 1
		System.out.println(a.get(2)); // zzzzzzzzz
		System.out.println(a.indexOf("b")); // 1
		System.out.println(a.indexOf(null)); // -1
		System.out.println(a.indexOf("Joe")); // -1
		a.add("b");
		a.add("b");
		a.add("b");
		a.add("e");
		a.add("b");
		a.add(null);
		System.out.println(a);
		System.out.println(a.indexOf("b")); // 2
		System.out.println(a.indexOf(null)); // -1
		System.out.println(a.indexOf("Joe")); // -1
		a.clear();
		System.out.println(a.size()); // Right Answer: 0
		System.out.println(a.isEmpty()); // Right Answer: True

	}
}
