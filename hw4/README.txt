Q1.Why is it important to #ifdef out methods and data structures that aren’t used for different versions of randtrack?

Methods and data structures that aren’t used occupy memory from the data and code. By surrounding these with #ifdef, the compiler doesn’t compile them and makes the program faster by saving memory.

Q2. How difficult was using TM compared to implementing global lock above?

It required a little less code that implementing a global lock as the global lock creation, initialization, lock and unlock code was eliminated. The critical section already identified in the global_lock code, surrounded by lock and unlock code lines, was replaced by __transactional_atomic{}. It’s a little easier in comparison.

Q3. Can you implement this without modifying the hash class, or without knowing its internal implementation?

No, this cannot be implemented without modifying the hash class or without knowing its internal implementation. This requires implementing locks to each list in the hash table, which can only be accessed at the list level. This can be only be achieved by modifying the hash class.

Q4. Can you properly implement this solely by modifying the hash class methods lookup and
insert ? Explain.

No, this cannot be done as there is a chance for race condition and inconsistency. For instance, two threads can lookup the value for the same key one after the other, as a result of list lock, in the hash table and find it’s empty. They then both proceed to insert a value into the hash table one after the other, as a result of list lock. This shall result in double insertion, which is unintended.

In addition, an inconsistency on the counter ( s->count++) may occur if two threads are accessing the same element and incrementing a counter. The outside counter cannot be made atomic solely by modifying internal lookup and insert functions.

Q5. Can you implement this by adding to the hash class a new function lookup_and_insert_if absent? Explain.

Yes the first problem mentioned in Q4 can be solved by the implementation of this new function through performing lookup and insert together atomically. However, since this function is implemented at the list level, the counter problem is not still resolved.

Q6. Can you implement it by adding new methods to the hash class lock_list and unlock_list ? Explain. Implement the simplest solution above that works (or a better one if you can think of one).

Yes these methods allow implementing locks surrounding the counter section and solves the second problem mentioned in Q4. In addition, they also solve the first problem if the whole critical section is locked using them as this only locks one list.

Q7. How difficult was using TM compared to implementing list locking above?

Implementing TM was much easier than list locking above as there was no need to modifying internal implementation of the hash.h by adding lock_list and unlock_list functions. 

Q8. 
Q9.
Q10.
Q11.
Q12.

