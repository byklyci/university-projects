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
            session_start();
            // List records
            $sql = 'CALL incomplete("' . $_SESSION['ID'] . '")';
            $result = $conn->query($sql);

            if ($result->num_rows > 0) {
                ?>
                <table border = 1>
                    <tr>
                        <th>ID</th>
                        <th>Name</th>
                        <th>Start Date</th>
                    </tr>
                <?php

                // output data of each row
                while($row = $result->fetch_assoc()) {
                    ?>
                    <tr>
                        <td><?php echo $row["ID"]; ?></td>
                        <td><?php echo $row["PName"]; ?></td>
                        <td><?php echo $row["StartDate"]; ?></td>
                    </tr>
                    <?php
                }

                ?>
                </table>
                <?php
            } else {
                echo "The table is empty";
            }
            echo "<p><a href = 'index.php'>Go back</a></p>";
        }
        $conn->close();
    ?>

</body>
</html>
