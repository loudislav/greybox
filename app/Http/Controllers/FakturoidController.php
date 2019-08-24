<?php

namespace App\Http\Controllers;

abstract class FakturoidController extends Controller
{
    /*
     * @return Fakturoid\Client
     */
    public function getFakturoidClient()
    {
        return new \Fakturoid\Client(getenv('FAKTUROID_SLUG'), getenv('FAKTUROID_EMAIL'), getenv('FAKTUROID_API_KEY'), getenv('FAKTUROID_USER_AGENT'));
    }

    public function fillInvoiceData(array $data, $fakturoidInvoice)
    {
        $data['fakturoid_id'] = $fakturoidInvoice->id;
        $data['number'] = $fakturoidInvoice->number;
        $data['status'] = $fakturoidInvoice->status;
        $data['issued_on'] = $fakturoidInvoice->issued_on;
        $data['due_on'] = $fakturoidInvoice->due_on;
        $data['currency'] = $fakturoidInvoice->currency;
        $data['language'] = $fakturoidInvoice->language;
        $data['total'] = $fakturoidInvoice->total;
        $data['paid_amount'] = $fakturoidInvoice->paid_amount;

        return $data;
    }
}
