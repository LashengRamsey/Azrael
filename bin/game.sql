/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`game` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `game`;

/*Table structure for table `account` */

DROP TABLE IF EXISTS `account`;

CREATE TABLE `account` (
  `Account` char(100) NOT NULL DEFAULT '""',
  `Lv` int(11) DEFAULT '0',
  `Exp` int(11) DEFAULT '0',
  `Gold` int(11) DEFAULT '0',
  `VipLv` int(11) DEFAULT '0',
  `VipExp` int(11) DEFAULT '0',
  `LastLoginTime` bigint(20) DEFAULT '0',
  `CreateTime` bigint(20) DEFAULT '0',
  `DATA` longtext,
  PRIMARY KEY (`Account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `account` */

/*Table structure for table `test` */

DROP TABLE IF EXISTS `test`;

CREATE TABLE `test` (
  `tid` bigint(20) NOT NULL DEFAULT '0',
  `name` char(100) DEFAULT NULL,
  PRIMARY KEY (`tid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `test` */

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
