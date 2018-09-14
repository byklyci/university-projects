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

            <a href = "create_project.php">Create a new record</a><br />

            <?php

            // List records
            $sql = "SELECT ID, PName, StartDate FROM project";
            $result = $conn->query($sql);

            if ($result->num_rows > 0) {
                ?>
                <table border = 1>
                    <tr>
                        <th>Operations</th>
                        <th>ID</th>
                        <th>Name</th>
                        <th>Start Date</th>
                    </tr>
                <?php

                // output data of each row
                while($row = $result->fetch_assoc()) {
                    ?>
                    <tr>
                        <td>
                            <a href = "delete_project.php?id=<?php echo $row["ID"]; ?>"><img src = "img/delete.png" alt = "Delete" /></a>
                            <a href = "edit_project.php?id=<?php echo $row["ID"]; ?>"><img src = "img/edit.png" alt = "Edit" /></a>
                        </td>
                        <td><?php echo $row["ID"]; ?></td>
                        <td><?php echo $row["PName"]; ?></td>
                        <td><?php echo $row["StartDate"]; ?></td>
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
