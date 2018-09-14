<html>
<head>
</head>
<body>

    <?php
        $servername = "localhost";
        $username = "root";
        $password = "";
        $dbname = "cmpe321hw3";

        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);

        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        }else{

            ?>

            <a href = "create_dotask.php">Create a new record</a><br />

            <?php

            // List records
            session_start();
            $sql = "SELECT TaskID, EmployeeID FROM dotask WHERE TaskID IN ( SELECT ID as TaskID FROM task WHERE ProjectID IN (SELECT ProjectID FROM manageproject WHERE ProjectManagerID = " . $_SESSION['ID'] . "))";
            $result = $conn->query($sql);

            if ($result->num_rows > 0) {
                ?>
                <table border = 1>
                    <tr>
                        <th>Operations</th>
                        <th>Task ID</th>
                        <th>Employee ID</th>
                    </tr>
                <?php

                // output data of each row
                while($row = $result->fetch_assoc()) {
                    ?>
                    <tr>
                        <td>
                            <a href = "delete_dotask.php?taskid=<?php echo $row["TaskID"]; ?>&amp;employeeid=<?php echo $row["EmployeeID"]; ?>"><img src = "img/delete.png" alt = "Delete" /></a>
                            <a href = "edit_dotask.php?taskid=<?php echo $row["TaskID"]; ?>&amp;employeeid=<?php echo $row["EmployeeID"]; ?>"><img src = "img/edit.png" alt = "Edit" /></a>
                        </td>
                        <td><?php echo $row["TaskID"]; ?></td>
                        <td><?php echo $row["EmployeeID"]; ?></td>
                    </tr>
                    <?php
                }

                ?>
                </table>
                <p><a href = 'index.php'>Go back</a></p>
                <?php
            } else {
                echo "The table is empty";
                echo "<p><a href = 'index.php'>Go back</a></p>";
            }
        }
        $conn->close();
    ?>

</body>
</html>
