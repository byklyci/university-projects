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

            <a href = "create_task.php">Create a new record</a><br />

            <?php

            // List records
            session_start();
            $sql = "SELECT ID, ProjectID, StartDate, DayItTakes FROM task WHERE ProjectID IN (SELECT ProjectID FROM manageproject WHERE ProjectManagerID = " . $_SESSION['ID'] . ")";
            $result = $conn->query($sql);

            if ($result->num_rows > 0) {
                ?>
                <table border = 1>
                    <tr>
                        <th>Operations</th>
                        <th>ID</th>
                        <th>Project ID</th>
                        <th>Start Date</th>
                        <th>Day It Takes</th>
                    </tr>
                <?php

                // output data of each row
                while($row = $result->fetch_assoc()) {
                    ?>
                    <tr>
                        <td>
                            <a href = "delete_task.php?id=<?php echo $row["ID"]; ?>&amp;projectid=<?php echo $row["ProjectID"]; ?>"><img src = "img/delete.png" alt = "Delete" /></a>
                            <a href = "edit_task.php?id=<?php echo $row["ID"]; ?>&amp;projectid=<?php echo $row["ProjectID"]; ?>"><img src = "img/edit.png" alt = "Edit" /></a>
                        </td>
                        <td><?php echo $row["ID"]; ?></td>
                        <td><?php echo $row["ProjectID"]; ?></td>
                        <td><?php echo $row["StartDate"]; ?></td>
                        <td><?php echo $row["DayItTakes"]; ?></td>
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
