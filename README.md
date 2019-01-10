TETRIS
===

---

**TERM**
---
2018.05.06 ~ 2018.06.03

---

**1. PLAY TETRIS**
---


* Block falls every seconds.

* Rotate or Move block following input direction.

* Get score according to the area the bottom of the falling block touch.

* When the row of field fill, delete this row and get score. 



**2. RANKING SYSTEM**
---


* Build data structure by reading rank.txt file.

* Sort data in nondecreasing order.

* Print ranking of selected range.

* Delete the ranking data according to input.

* Print one's scores in nondecreasing order.

* Before Finishing the program, store data structure in rank.txt file.



**3. RECOMMENDED PLAY**
---

* Recommend position by using tree.

* Automatic play mode that chooses position it expects to lead to the highest score.

---

**IMPLEMENTATION**
---


<img src="https://user-images.githubusercontent.com/30820487/50970843-e0bd8200-1525-11e9-968d-970fad3c8272.png" width="30%" height="30%">

<img src="https://user-images.githubusercontent.com/30820487/50970586-2168cb80-1525-11e9-80b7-c0167d99aa2e.png" width="50%" height="50%">

<img src="https://user-images.githubusercontent.com/30820487/50970625-3b0a1300-1525-11e9-93e2-4091d08fd5d6.png" width="50%" height="50%">

<img src="https://user-images.githubusercontent.com/30820487/50970658-57a64b00-1525-11e9-965e-15b2255793ed.png" width="50%" height="50%">

<img src="https://user-images.githubusercontent.com/30820487/50970854-ed41da80-1525-11e9-98c1-ffbc6ec7cd56.png" width="40%" height="40%">

---

**IDEA FOR RANKING SYSTEM**
---

*DATA STRUCTURE*

<img src="https://user-images.githubusercontent.com/30820487/50973733-eec2d100-152c-11e9-93fb-e9a00df6b868.png" width="40%" height="40%">

"search" is defined constant.

<img src="https://user-images.githubusercontent.com/30820487/50973835-334e6c80-152d-11e9-8b47-8ca72949670c.png" width="50%" height="50%">

Wine circle sign indicates the node that is stored in "Head"->link.

Unlike arrays, Linked-list has disadvantage of not being able to refer to intermediate node.

By using above data structure, I tried to solving this advantage.

*ALGORITHM*

* Insertion / Deletion:

  * Find the approximate position by looking over "Head->link" list.

  * Start from one of "Head->link" list, find the exact position.

  * After insertion or deletion , adjust "Head->link" list.


* Print ranking of selected range(x,y):

  * Find the approximate x position by looking over "Head->link" list.

  * Start from one of "Head->link" list, find the exact x position.

  * From x position, print node information as following the linked-list until arriving at y.


---

**IDEA FOR AUTOMATIC PLAY**
---

*DATA STRUCTURE*

<img src="https://user-images.githubusercontent.com/30820487/50974578-e8cdef80-152e-11e9-8287-34aae11196e4.png" width="40%" height="40%">

**lv**: depth of node in tree.

**max_h**: maximum height of the blocks.

**num array**: store the number of blocks for each height.

**h array**: store the number of blocks for each col.



**Example**

<img src="https://user-images.githubusercontent.com/30820487/50994142-d4a1e680-155e-11e9-83c5-26e5e643dd26.png" width="60%" height="60%">

<img src="https://user-images.githubusercontent.com/30820487/50975301-942b7400-1530-11e9-9dd7-4b20b13b2e0f.png" width="50%" height="50%">


Without storing entire field, I can expect how the block stacks up by looking over "h" array and whether or not any row can be full by looking over "num" array.

I tried to come up with node structure that uses memory efficiently.


*ALGORITHM*

* Do deeper predictions only about the next situation that is judged to result in a high score.

* In this process, I did not build tree. Instead, by recursive function call, I anticipate the dipper prediction.

* I accept the potential if...

  * delete one or more lines

  * make blank less than or equal to that of the status of max score.

  * generate score that is higher than the previous max score

  * generate score that is equal to max score but create smaller number of blank spaces or lower max_h.
