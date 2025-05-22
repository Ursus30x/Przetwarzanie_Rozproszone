import java.util.*;

class Resource<T> {
    private final Queue<T> buffer = new LinkedList<>();
    private final int capacity;

    public Resource(int capacity) {
        this.capacity = capacity;
    }

    public synchronized void produce(T item) throws InterruptedException {
        while (buffer.size() >= capacity) wait();
        buffer.add(item);
        System.out.println(Thread.currentThread().getName() + " produced: " + item);
        notifyAll();
    }

    public synchronized T consume() throws InterruptedException {
        while (buffer.isEmpty()) wait();
        T item = buffer.poll();
        System.out.println(Thread.currentThread().getName() + " consumed: " + item);
        notifyAll();
        return item;
    }

    public int getCapacity(){
        return capacity;
    }
}

// Producent - np. kopalnia
class Producer extends Thread {
    private final Resource<String> resource;
    private final String item;
    private final int delay;

    public Producer(String name, Resource<String> resource, String item, int delay) {
        super(name);
        this.resource = resource;
        this.item = item;
        this.delay = delay;
    }

    public void run() {
        try {
            while (true) {
                resource.produce(item);
                Thread.sleep(delay);
            }
        } catch (InterruptedException e) {
            System.out.println(getName() + " ended.");
        }
    }
}

// Konsument - np. księżniczka
class Consumer extends Thread {
    private final Resource<String> resource;
    private final int delay;

    public Consumer(String name, Resource<String> resource, int delay) {
        super(name);
        this.resource = resource;
        this.delay = delay;
    }

    public void run() {
        try {
            while (true) {
                resource.consume();
                Thread.sleep(delay);
            }
        } catch (InterruptedException e) {
            System.out.println(getName() + " ended.");
        }
    }
}

class ProducerConsumer extends Thread {
    private final Resource<String> input;
    private final Resource<String> output;
    private final String inputItemName;
    private final String outputItemName;
    private final int delay;

    public ProducerConsumer(String name, Resource<String> input, String inputItemName,
                            Resource<String> output, String outputItemName, int delay) {
        super(name);
        this.input = input;
        this.output = output;
        this.inputItemName = inputItemName;
        this.outputItemName = outputItemName;
        this.delay = delay;
    }

    public void run() {
        try {
            while (true) {
                input.consume(); // e.g. "Coal"
                output.produce(outputItemName); // e.g. "Weapon"
                Thread.sleep(delay);
            }
        } catch (InterruptedException e) {
            System.out.println(getName() + " ended.");
        }
    }
}



// Armia walcząca
class Army extends Thread {
    private final Kingdom own;
    private final Kingdom enemy;
    private int power = 0;
    private final Random random = new Random();

    private static boolean battleInProgress = false; // Wspólna flaga

    public Army(Kingdom own, Kingdom enemy) {
        super(own.name + " Army");
        this.own = own;
        this.enemy = enemy;
    }

    public synchronized int getPower() {
        return power;
    }

    public synchronized void reducePower(int damage) {
        power -= damage;
        System.out.println(getName() + " lost " + damage + " power! Remaining: " + power);
    }

    private boolean consumeBeerSafely(Kingdom k) {
        try {
            if (random.nextDouble() < 0.5) return false; // 50% chance to skip morale
            k.beer.consume();
            return true;
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            return false;
        }
    }



    public void run() {
        try {
            while (true) {
                own.weapon.consume();
                own.food.consume();
                synchronized (this) {
                    power++;
                }
                Thread.sleep(500);

                if (getPower() >= 10 && enemy.army.getPower() >= 10) {
                    tryBattle();
                }

                if (getPower() <= 0) {
                    System.out.println(getName() + " has been defeated!");
                    enemy.stopAll("🏆 " + enemy.name + " wins the war!");
                    own.stopAll(own.name + " looses the war...");
                    break;
                }
            }
        } catch (InterruptedException e) {
            System.out.println(getName() + " ended.");
        }
    }

    private void tryBattle() throws InterruptedException {
        synchronized (Army.class) {
            if (battleInProgress) return; // jeśli trwa inna bitwa, nic nie rób
            battleInProgress = true;
        }

        try {
            battle();
        } finally {
            synchronized (Army.class) {
                battleInProgress = false; // zakończenie bitwy
            }
        }
    }

    private void battle() throws InterruptedException {
        System.out.println("------------------------------------------------------------------------------------");
        System.out.println("⚔️ Battle between " + own.name + " and " + enemy.name + " begins!");

        boolean ownHasBeer = consumeBeerSafely(own);
        boolean enemyHasBeer = consumeBeerSafely(enemy);

        double ownMorale = ownHasBeer ? 1.5 : 1.0;
        double enemyMorale = enemyHasBeer ? 1.5 : 1.0;

        if (ownHasBeer) System.out.println(own.name + " army boosted by beer! 🍺");
        if (enemyHasBeer) System.out.println(enemy.name + " army boosted by beer! 🍺");

        int diff = Math.abs(this.power - enemy.army.power);

        int myAttack1 = (int)(random.nextInt(5 + diff) * ownMorale);
        int myAttack2 = (int)(random.nextInt(5 + diff) * ownMorale);
        int myDefense1 = (int)(random.nextInt(3 + diff) * ownMorale);
        int myDefense2 = (int)(random.nextInt(3 + diff) * ownMorale);

        int enemyAttack1 = (int)(random.nextInt(5 + diff) * enemyMorale);
        int enemyAttack2 = (int)(random.nextInt(5 + diff) * enemyMorale);
        int enemyDefense1 = (int)(random.nextInt(3 + diff) * enemyMorale);
        int enemyDefense2 = (int)(random.nextInt(3 + diff) * enemyMorale);

        int damageToEnemy = Math.max(0, myAttack1 - enemyDefense1) + Math.max(0, myAttack2 - enemyDefense2);
        int damageToMe = Math.max(0, enemyAttack1 - myDefense1) + Math.max(0, enemyAttack2 - myDefense2);

        enemy.army.reducePower(damageToEnemy);
        this.reducePower(damageToMe);

        System.out.println("------------------------------------------------------------------------------------");
        Thread.sleep(1000);
    }

}


// Królestwo
class Kingdom {
    final String name;
    final Resource<String> coal = new Resource<>(5);
    final Resource<String> food = new Resource<>(5);
    final Resource<String> weapon = new Resource<>(5);
    final Resource<String> beer = new Resource<>(5);
    final List<Thread> threads = new ArrayList<>();
    Army army;

    public Kingdom(String name) {
        this.name = name;
    }

    public void startKingdom() {
        threads.add(new Producer(name + "-Mine", coal, "Coal", 700));
        threads.add(new Producer(name + "-Farm", food, "Food", 400));
        threads.add(new ProducerConsumer(name + "-Brewer", food, "Food", beer, "Beer", 3000));
        threads.add(new ProducerConsumer(name + "-Smith", coal, "Coal", weapon, "Weapon", 800));

        threads.forEach(Thread::start);
    }

    public void startWarWith(Kingdom other) {
        army = new Army(this, other);
        threads.add(army);
        army.start();
    }

    public void stopAll(String message) {
        System.out.println(message);
        threads.forEach(Thread::interrupt);
    }
}


public class KingdomSimulation {
    public static void main(String[] args) {
        Kingdom k1 = new Kingdom("Wielkie Ksiestwo Lechii");
        Kingdom k2 = new Kingdom("Super krulestwo");

        k1.startKingdom();
        k2.startKingdom();

        k1.startWarWith(k2);
        k2.startWarWith(k1);
    }
}
