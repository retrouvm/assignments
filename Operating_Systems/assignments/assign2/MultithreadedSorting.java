//package Operating_Systems.assignments.assign2;

import java.util.Random;

//import Operating_Systems.assignments.assign2.MergingThread;
//import Operating_Systems.assignments.assign2.SortingThread;

public class MultithreadedSorting {
    private double[] A;
    private double[] B;
    private double[] C;
    private double[] Afirsthalf;
    private double[] Asecondhalf;
    private double AfirstAvg;
    private double AsecondAvg;
    private int N;
    private final Object lock = new Object();

    public MultithreadedSorting(int N) {
        this.N = N;
        A = new double[N];
        B = new double[N];
        C = new double[N];
        Afirsthalf = new double[N / 2];
        Asecondhalf = new double[N / 2];
    }

    public static void main(String[] args) {
        int N = Integer.parseInt(args[0]);
        MultithreadedSorting sorter = new MultithreadedSorting(N);
        sorter.execute();
    }
    public void execute() {
        Random rand = new Random();
        for (int i = 0; i < N; i++) {
            A[i] = 1.0 + rand.nextDouble() * 999.0;
        }
        System.arraycopy(A, 0, B, 0, N);
        long startTime = System.nanoTime();

        Thread thB = new Thread(new SortingThread(0, N - 1, B, this));
        thB.start();
        try {
            thB.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        long endTime = System.nanoTime();
        System.out.printf("Sorting by ONE thread is done in %.2f ms%n", (endTime - startTime) / 1e6);
        // ... (previous portion)

        System.arraycopy(A, 0, Afirsthalf, 0, N / 2);
        System.arraycopy(A, N / 2, Asecondhalf, 0, N / 2);

        startTime = System.nanoTime();

        Thread thA1 = new Thread(new SortingThread(0, N / 2 - 1, Afirsthalf, this));
        Thread thA2 = new Thread(new SortingThread(0, N / 2 - 1, Asecondhalf, this));
        thA1.start();
        thA2.start();

        try {
            thA1.join();
            thA2.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        Thread thM = new Thread(new MergingThread(this));
        thM.start();

        try {
            thM.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        endTime = System.nanoTime();
        System.out.printf("Sorting by TWO threads is done in %.2f ms%n", (endTime - startTime) / 1e6);
        System.out.printf("Average of merged array: %.2f%n", (AfirstAvg + AsecondAvg) / 2);

        // Print at least the first 10 values of the sorted array
        System.out.print("First 10 values of the merged array: ");
        for (int i = 0; i < Math.min(10, C.length); i++) {
            System.out.print(C[i] + " ");
        }
        System.out.println();
    }

    public void setAfirstAvg(double avg) {
        synchronized(lock) {
            this.AfirstAvg = avg;
        }
    }

    public void setAsecondAvg(double avg) {
        synchronized(lock) {
            this.AsecondAvg = avg;
        }
    }

    public void merge() {
        int i = 0, j = 0, k = 0;
        while (i < Afirsthalf.length && j < Asecondhalf.length) {
            if (Afirsthalf[i] <= Asecondhalf[j]) {
                C[k++] = Afirsthalf[i++];
            } else {
                C[k++] = Asecondhalf[j++];
            }
        }

        while (i < Afirsthalf.length) {
            C[k++] = Afirsthalf[i++];
        }

        while (j < Asecondhalf.length) {
            C[k++] = Asecondhalf[j++];
        }
    }
    public void insertionSort(double[] arr, int start, int end) {
        for (int i = start + 1; i <= end; i++) {
            double key = arr[i];
            int j = i - 1;
            while (j >= start && arr[j] > key) {
                arr[j + 1] = arr[j];
                j = j - 1;
            }
            arr[j + 1] = key;
        }
    }
}
