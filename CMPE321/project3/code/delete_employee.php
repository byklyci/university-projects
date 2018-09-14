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

            // Fetch the record
            $sql = "SELECT ID, FName, LName FROM employee WHERE ID = " . $_GET['id'];
            $result = $conn->query($sql);

            // If the record actually exists
            if ($result->num_rows > 0) {
                ?>
                <form action="delete_employee_result.php" method="post">
                <?php

                // Get the data
                $row = $result->fetch_assoc();
                ?>
                    Are you sure you want to delete the following record? <br />
                    <p>ID: <input type="text" name="ID" value = "<?php echo $row["ID"] ?>" readonly /></p>
                    <p>First name: <input type="text" name="FName" value = "<?php echo $row["FName"] ?>" readonly /></p>
                    <p>Last name: <input type="text" name="LName" value = "<?php echo $row["LName"] ?>" readonly /></p>
                    <p><input type="submit" value = "Delete Record" /></p>
                </form>
                <?php
            } else {
                echo "Record does not exist";
            }
        }
        $conn->close();
    ?>

</body>
</html>
