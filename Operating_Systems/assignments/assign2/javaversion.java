//package Operating_Systems.assignments.assign2;
//import java.util.Random;
//import java.util.concurrent.TimeUnit;


class SortingThread implements Runnable {
    private final int start;
    private final int end;
    private final double[] array;
    private final MultithreadedSorting sorter;

    public SortingThread(int start, int end, double[] array, MultithreadedSorting sorter) {
        this.start = start;
        this.end = end;
        this.array = array;
        this.sorter = sorter;
    }

    @Override
    public void run() {
        sorter.insertionSort(array, start, end);
        double sum = 0;
        for (int i = start; i <= end; i++) {
            sum += array[i];
        }

        if (start == 0) {
            sorter.setAfirstAvg(sum / (end - start + 1));
        } else {
            sorter.setAsecondAvg(sum / (end - start + 1));
        }
    }
}
class MergingThread implements Runnable {
    private final MultithreadedSorting sorter;

    public MergingThread(MultithreadedSorting sorter) {
        this.sorter = sorter;
    }

    @Override
    public void run() {
        // Logic for merging sorted arrays
        sorter.merge();
    }
}

