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

            <a href = "create_manageproject.php">Create a new record</a><br />

            <?php

            // List records
            $sql = "SELECT ProjectID, ProjectManagerID FROM manageproject";
            $result = $conn->query($sql);

            if ($result->num_rows > 0) {
                ?>
                <table border = 1>
                    <tr>
                        <th>Operations</th>
                        <th>Project ID</th>
                        <th>Project Manager ID</th>
                    </tr>
                <?php

                // output data of each row
                while($row = $result->fetch_assoc()) {
                    ?>
                    <tr>
                        <td>
                            <a href = "delete_manageproject.php?projectid=<?php echo $row["ProjectID"]; ?>&amp;projectmanagerid=<?php echo $row["ProjectManagerID"]; ?>"><img src = "img/delete.png" alt = "Delete" /></a>
                            <a href = "edit_manageproject.php?projectid=<?php echo $row["ProjectID"]; ?>&amp;projectmanagerid=<?php echo $row["ProjectManagerID"]; ?>"><img src = "img/edit.png" alt = "Edit" /></a>
                        </td>
                        <td><?php echo $row["ProjectID"]; ?></td>
                        <td><?php echo $row["ProjectManagerID"]; ?></td>
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
