//
//  ViewController.swift
//  SDK_Demo
//
//  Created by pxh on 2016/10/12.
//  Copyright © 2016年 pxh. All rights reserved.
//

import UIKit
let cellIndentifier:String = "MainTableViewCell"
class ViewController: UIViewController,UITableViewDelegate,UITableViewDataSource{
    var itmes : NSMutableArray?
    @IBOutlet weak var tableView: UITableView!
    @IBOutlet weak var tipLabel: UILabel!
    @IBOutlet weak var tipBg: UIView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        USC_SDK.logTest()
        let uscsdk = USC_SDK()
        uscsdk.logTestTwo()
        self.resetData()
        self.setNavigation()
        self.addMainTableView()
    }
    func resetData() {
        self.itmes = NSMutableArray.init(array: ["语音理解","声纹识别"])
    }
    func setNavigation(){
        self.title = "云知声只能语音交互平台"
        self.tipBg.layer.borderWidth = 1
        self.tipBg.layer.cornerRadius = 5
        self.tipBg.layer.borderColor = UIColor.black.cgColor
    }
    func addMainTableView(){
        self.tableView!.delegate = self;
        self.tableView!.dataSource = self;
        self.tableView.register(UITableViewCell.classForCoder(), forCellReuseIdentifier: cellIndentifier)
        self.view.addSubview(self.tableView!)
    }
    //Mark: UITableviewDelegate

    func tableView(_ tableView: UITableView, didEndDisplayingHeaderView view: UIView, forSection section: Int){
    
    }
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath){
        switch indexPath.row {
        case 0:
            let asrStoryBoard = UIStoryboard.init(name: "USCAsrViewController", bundle: Bundle.main)
            self.navigationController?.pushViewController(asrStoryBoard.instantiateViewController(withIdentifier: "USCAsrViewController"), animated: true)
        case 1:
            let vprStoryBoard = UIStoryboard.init(name: "USCVoiceprintViewController", bundle: Bundle.main)
            self.navigationController?.pushViewController(vprStoryBoard.instantiateViewController(withIdentifier: ""), animated: true)
        default: break
        }
    }
    //Mark : UITableviewDataSourse
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return (self.itmes?.count)!
    }
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell:UITableViewCell = tableView.dequeueReusableCell(withIdentifier: cellIndentifier, for: indexPath)
        let title = self.itmes?[indexPath.row]
        cell.textLabel?.text = title as! String?
        return cell
    }
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

